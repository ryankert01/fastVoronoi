"""
Basic example: Creating a Voronoi diagram from simple points
"""

import numpy as np
import fastvoronoi

# Create some example points
points = np.array([
    [1.0, 5.0],
    [3.0, 1.0],
    [8.0, 6.0],
    [5.0, 4.0],
    [2.0, 7.0]
])

# Build Voronoi diagram
print("Building Voronoi diagram from {} points...".format(len(points)))
voronoi = fastvoronoi.Voronoi(points)

# Display basic information
print("\nVoronoi Diagram Information:")
print(f"  Number of cells: {len(voronoi.cells)}")
print(f"  Number of edges: {len(voronoi.edges)}")
print(f"  Number of vertices: {len(voronoi.vertices)}")

# Find nearest generator point for query points
query_points = np.array([
    [2.0, 3.0],
    [6.0, 5.0]
])

nearest = voronoi.find_nearest(query_points)
print("\nNearest neighbor queries:")
for i, (query, idx) in enumerate(zip(query_points, nearest)):
    print(f"  Query {query} -> Generator {idx} at {points[idx]}")

# Plot the diagram
print("\nPlotting diagram...")
voronoi.plot()

import matplotlib.pyplot as plt
plt.savefig('voronoi_basic.png', dpi=150, bbox_inches='tight')
print("Saved to 'voronoi_basic.png'")
plt.show()
