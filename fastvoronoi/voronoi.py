"""
Voronoi diagram class with GeoPandas integration
"""

import numpy as np
from typing import Optional, Union, List, Tuple
try:
    import geopandas as gpd
    from shapely.geometry import Point as ShapelyPoint, Polygon, MultiPolygon
    GEOPANDAS_AVAILABLE = True
except ImportError:
    GEOPANDAS_AVAILABLE = False
    gpd = None
    ShapelyPoint = None
    Polygon = None
    MultiPolygon = None

try:
    import matplotlib.pyplot as plt
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False
    plt = None

try:
    import contextily as ctx
    CONTEXTILY_AVAILABLE = True
except ImportError:
    CONTEXTILY_AVAILABLE = False
    ctx = None

try:
    from _fastvoronoi import VoronoiDiagram as _VoronoiDiagram
    from _fastvoronoi import Point as _Point
    NATIVE_AVAILABLE = True
except ImportError:
    NATIVE_AVAILABLE = False
    _VoronoiDiagram = None
    _Point = None


class Voronoi:
    """
    A class to compute bounded Voronoi diagrams for geospatial analysis.
    
    This class provides a high-level interface for creating Voronoi diagrams
    from various input formats and integrates seamlessly with GeoPandas workflows.
    
    Parameters
    ----------
    points : gpd.GeoDataFrame or np.ndarray or list of tuples
        Generator points for the Voronoi diagram.
        - If GeoDataFrame, the 'geometry' column is used
        - If numpy array, should be Nx2 array of (x, y) coordinates
        - If list, should be list of (x, y) tuples
    boundary : Polygon, optional
        Optional boundary polygon to clip the Voronoi diagram.
        If None, the diagram extends to infinity (or a large bounding box).
    
    Attributes
    ----------
    points : np.ndarray
        The generator points as Nx2 numpy array
    boundary : Polygon or None
        The boundary polygon used for clipping
    _diagram : _VoronoiDiagram
        The underlying C++ Voronoi diagram object
    
    Examples
    --------
    >>> import fastvoronoi
    >>> import numpy as np
    >>> points = np.array([[1, 5], [3, 1], [8, 6]])
    >>> v = fastvoronoi.Voronoi(points)
    >>> v.plot()
    """
    
    def __init__(
        self,
        points: Union['gpd.GeoDataFrame', np.ndarray, List[Tuple[float, float]]],
        boundary: Optional['Polygon'] = None
    ):
        """
        Initialize and build the Voronoi diagram.
        
        Raises
        ------
        ImportError
            If required dependencies are not available
        ValueError
            If input points are invalid
        """
        if not NATIVE_AVAILABLE:
            raise ImportError(
                "Native C++ extension not available. "
                "Please build the extension with: pip install -e ."
            )
        
        # Convert points to numpy array
        self.points = self._convert_points(points)
        self.boundary = boundary
        self._original_crs = None
        
        # Build the Voronoi diagram
        self._build_diagram()
    
    def _convert_points(
        self, 
        points: Union['gpd.GeoDataFrame', np.ndarray, List[Tuple[float, float]]]
    ) -> np.ndarray:
        """Convert various point formats to numpy array."""
        if GEOPANDAS_AVAILABLE and isinstance(points, gpd.GeoDataFrame):
            # Extract coordinates from GeoDataFrame
            self._original_crs = points.crs
            coords = []
            for geom in points.geometry:
                if isinstance(geom, ShapelyPoint):
                    coords.append([geom.x, geom.y])
                else:
                    raise ValueError("GeoDataFrame must contain Point geometries")
            return np.array(coords)
        
        elif isinstance(points, np.ndarray):
            if points.ndim != 2 or points.shape[1] != 2:
                raise ValueError("Numpy array must be Nx2 shape")
            return points
        
        elif isinstance(points, list):
            return np.array(points)
        
        else:
            raise TypeError(
                "Points must be GeoDataFrame, numpy array, or list of tuples"
            )
    
    def _build_diagram(self):
        """Build the C++ Voronoi diagram."""
        # Convert numpy array to C++ Point objects
        cpp_points = []
        for i, (x, y) in enumerate(self.points):
            cpp_points.append(_Point(float(x), float(y), i))
        
        # Build diagram
        self._diagram = _VoronoiDiagram(cpp_points)
        
        # Clip to boundary if provided
        if self.boundary is not None:
            boundary_coords = list(self.boundary.exterior.coords)
            boundary_points = [_Point(x, y) for x, y in boundary_coords]
            self._diagram.clip_to_boundary(boundary_points)
    
    def to_geodataframe(self) -> 'gpd.GeoDataFrame':
        """
        Convert the Voronoi cells to a GeoDataFrame.
        
        Each row represents a Voronoi cell with its polygon geometry
        and an ID linking back to the original generator point.
        
        Returns
        -------
        gpd.GeoDataFrame
            GeoDataFrame containing Voronoi cells
        
        Raises
        ------
        ImportError
            If GeoPandas is not installed
        
        Examples
        --------
        >>> v = Voronoi(points)
        >>> gdf = v.to_geodataframe()
        >>> print(gdf.head())
        """
        if not GEOPANDAS_AVAILABLE:
            raise ImportError("GeoPandas is required for to_geodataframe()")
        
        cells = self._diagram.get_cells()
        
        geometries = []
        indices = []
        
        for cell in cells:
            # Convert cell vertices to Shapely Polygon
            if len(cell.vertices) >= 3:
                coords = [(v.x, v.y) for v in cell.vertices]
                poly = Polygon(coords)
                geometries.append(poly)
                indices.append(cell.site_index)
        
        gdf = gpd.GeoDataFrame(
            {'site_index': indices, 'geometry': geometries},
            crs=self._original_crs
        )
        
        return gdf
    
    def plot(self, with_basemap: bool = False, ax=None, **kwargs):
        """
        Visualize the Voronoi diagram.
        
        Parameters
        ----------
        with_basemap : bool, default False
            If True, overlay the diagram on a tile map (e.g., OpenStreetMap).
            Requires contextily package.
        ax : matplotlib.axes.Axes, optional
            Axes to draw on. If None, creates new figure.
        **kwargs
            Additional arguments passed to matplotlib plotting functions.
        
        Returns
        -------
        matplotlib.axes.Axes
            The axes containing the plot
        
        Raises
        ------
        ImportError
            If matplotlib is not installed
        
        Examples
        --------
        >>> v = Voronoi(points)
        >>> v.plot()
        >>> v.plot(with_basemap=True)  # With map background
        """
        if not MATPLOTLIB_AVAILABLE:
            raise ImportError("Matplotlib is required for plotting")
        
        if ax is None:
            fig, ax = plt.subplots(figsize=(10, 8))
        
        # Plot edges
        edges = self._diagram.get_edges()
        for edge in edges:
            x_coords = [edge.start.x, edge.end.x]
            y_coords = [edge.start.y, edge.end.y]
            ax.plot(x_coords, y_coords, 'b-', linewidth=1, alpha=0.6)
        
        # Plot generator points
        ax.plot(self.points[:, 0], self.points[:, 1], 'ro', 
                markersize=8, label='Generator Points')
        
        # Plot boundary if provided
        if self.boundary is not None:
            x, y = self.boundary.exterior.xy
            ax.plot(x, y, 'k-', linewidth=2, label='Boundary')
        
        # Add basemap if requested
        if with_basemap:
            if not CONTEXTILY_AVAILABLE:
                print("Warning: contextily not available, skipping basemap")
            elif not GEOPANDAS_AVAILABLE:
                print("Warning: GeoPandas required for basemap, skipping")
            elif self._original_crs is None:
                print("Warning: No CRS available, skipping basemap")
            else:
                try:
                    # Convert to Web Mercator for contextily
                    gdf = self.to_geodataframe()
                    if self._original_crs != 'EPSG:3857':
                        gdf = gdf.to_crs('EPSG:3857')
                    
                    ctx.add_basemap(ax, crs=gdf.crs.to_string(), 
                                   source=ctx.providers.OpenStreetMap.Mapnik)
                except Exception as e:
                    print(f"Warning: Could not add basemap: {e}")
        
        ax.set_aspect('equal')
        ax.legend()
        ax.set_title('Voronoi Diagram')
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.grid(True, alpha=0.3)
        
        return ax
    
    def find_nearest(
        self, 
        target_points: Union[np.ndarray, List[Tuple[float, float]]]
    ) -> np.ndarray:
        """
        Find the nearest generator point index for target points.
        
        For each target point, returns the index of the generator point
        whose Voronoi cell contains it (i.e., the nearest generator).
        
        Parameters
        ----------
        target_points : np.ndarray or list of tuples
            Query points as Nx2 array or list of (x, y) tuples
        
        Returns
        -------
        np.ndarray
            Array of indices of nearest generator points
        
        Examples
        --------
        >>> v = Voronoi(points)
        >>> targets = np.array([[2, 3], [5, 5]])
        >>> nearest = v.find_nearest(targets)
        >>> print(nearest)  # [0, 2]
        """
        if isinstance(target_points, list):
            target_points = np.array(target_points)
        
        if target_points.ndim == 1:
            target_points = target_points.reshape(1, -1)
        
        # Convert to C++ Point objects
        cpp_targets = [_Point(float(x), float(y)) for x, y in target_points]
        
        # Find nearest
        indices = self._diagram.find_nearest(cpp_targets)
        
        return np.array(indices)
    
    @property
    def cells(self):
        """Get the Voronoi cells."""
        return self._diagram.get_cells()
    
    @property
    def edges(self):
        """Get the Voronoi edges."""
        return self._diagram.get_edges()
    
    @property
    def vertices(self):
        """Get the Voronoi vertices."""
        return self._diagram.get_vertices()
