"""
GIS example: Using GeoPandas for geospatial analysis
"""

import geopandas as gpd
from shapely.geometry import Point, Polygon
import fastvoronoi

# Create geospatial data from locations of interest
# Example: Fire stations in a city
stations_data = {
    'name': ['Station A', 'Station B', 'Station C', 'Station D'],
    'geometry': [
        Point(1, 5),
        Point(3, 1),
        Point(8, 6),
        Point(5, 3)
    ]
}
stations_gdf = gpd.GeoDataFrame(stations_data, crs="EPSG:4326")

print("Fire Stations:")
print(stations_gdf)

# Define a city boundary as an Area of Interest (AOI)
city_boundary = Polygon([
    (0, 0), (10, 0), (10, 8), (0, 8)
])

print("\nBuilding Voronoi diagram for service areas...")

# Build the bounded Voronoi diagram directly from the GeoDataFrame
voronoi = fastvoronoi.Voronoi(stations_gdf, boundary=city_boundary)

# Get the result as a GeoDataFrame for further spatial analysis
service_areas_gdf = voronoi.to_geodataframe()

print("\nService Areas:")
print(service_areas_gdf)

# Add area calculation
service_areas_gdf['area'] = service_areas_gdf.geometry.area
print("\nService area sizes:")
for idx, row in service_areas_gdf.iterrows():
    station_name = stations_data['name'][row['site_index']]
    print(f"  {station_name}: {row['area']:.2f} square units")

# Visualize
print("\nPlotting service areas...")
voronoi.plot()

import matplotlib.pyplot as plt
plt.savefig('voronoi_gis.png', dpi=150, bbox_inches='tight')
print("Saved to 'voronoi_gis.png'")
plt.show()

# You can also export to file for use in GIS software
service_areas_gdf.to_file('service_areas.geojson', driver='GeoJSON')
print("Exported to 'service_areas.geojson'")
