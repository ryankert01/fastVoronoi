#!/usr/bin/env python3
"""
Visualize all C++ golden test cases from test_voronoi.cpp
Generates Voronoi diagrams using scipy.spatial.voronoi_plot_2d and saves them.
"""

import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import Voronoi, voronoi_plot_2d
import os

# Create output directory
output_dir = "golden_test_plots"
os.makedirs(output_dir, exist_ok=True)


def plot_voronoi(points, title, filename):
    """Plot and save a Voronoi diagram."""
    try:
        vor = Voronoi(points)

        plt.figure(figsize=(10, 8))
        voronoi_plot_2d(vor, show_vertices=True, line_colors="blue", point_size=10)

        plt.title(title, fontsize=14, fontweight="bold")
        plt.xlabel("X")
        plt.ylabel("Y")
        plt.grid(True, alpha=0.3)
        plt.axis("equal")

        # Save the plot
        filepath = os.path.join(output_dir, filename)
        plt.savefig(filepath, dpi=150, bbox_inches="tight")
        print(f"Saved: {filepath}")
        plt.close()
    except Exception as e:
        # Handle edge cases (e.g., collinear points)
        print(f"Warning: Could not generate Voronoi plot for {filename}: {str(e)[:100]}")
        plt.figure(figsize=(10, 8))
        plt.scatter(points[:, 0], points[:, 1], c="red", s=100, zorder=5)
        for i, p in enumerate(points):
            plt.text(p[0] + 0.05, p[1] + 0.05, f"P{i}", fontsize=12, fontweight="bold")
        plt.title(
            title + "\n(Edge case: No valid Voronoi diagram)",
            fontsize=14,
            fontweight="bold",
        )
        plt.xlabel("X")
        plt.ylabel("Y")
        plt.grid(True, alpha=0.3)
        plt.axis("equal")

        filepath = os.path.join(output_dir, filename)
        plt.savefig(filepath, dpi=150, bbox_inches="tight")
        print(f"Saved (edge case): {filepath}")
        plt.close()


# Test 1: SimpleTriangle
print("Generating Test 1: SimpleTriangle...")
points = np.array([[0.0, 0.0], [1.0, 0.0], [0.5, 1.0]])
plot_voronoi(points, "Test 1: Simple Triangle\n(0,0), (1,0), (0.5,1)", "01_simple_triangle.png")

# Test 2: SquarePoints
print("Generating Test 2: SquarePoints...")
points = np.array([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]])
plot_voronoi(points, "Test 2: Square Points\n(0,0), (1,0), (1,1), (0,1)", "02_square_points.png")

# Test 3: ThreeByThreeGrid
print("Generating Test 3: ThreeByThreeGrid...")
points = np.array(
    [
        [0.0, 0.0],
        [0.0, 1.0],
        [0.0, 2.0],
        [1.0, 0.0],
        [1.0, 1.0],
        [1.0, 2.0],
        [2.0, 0.0],
        [2.0, 1.0],
        [2.0, 2.0],
    ]
)
plot_voronoi(points, "Test 3: 3x3 Grid\n(Problem Statement Example)", "03_3x3_grid.png")

# Test 4: CollinearPoints
print("Generating Test 4: CollinearPoints...")
points = np.array([[0.0, 0.0], [1.0, 0.0], [2.0, 0.0]])
plot_voronoi(points, "Test 4: Collinear Points\n(0,0), (1,0), (2,0)", "04_collinear_points.png")

# Test 5: RegularPentagon
print("Generating Test 5: RegularPentagon...")
points = []
for i in range(5):
    angle = 2.0 * np.pi * i / 5.0
    points.append([np.cos(angle), np.sin(angle)])
points = np.array(points)
plot_voronoi(points, "Test 5: Regular Pentagon\n5 points on unit circle", "05_pentagon.png")

# Test 6: DeterministicCoordinates
print("Generating Test 6: DeterministicCoordinates...")
points = np.array([[0.0, 0.0], [2.0, 0.0], [1.0, 2.0]])
plot_voronoi(
    points,
    "Test 6: Deterministic Coordinates\n(0,0), (2,0), (1,2)\nCircumcenter at (1.0, 0.75)",
    "06_deterministic.png",
)

# Test 7: SquareWithCenter
print("Generating Test 7: SquareWithCenter...")
points = np.array([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0], [0.5, 0.5]])
plot_voronoi(
    points,
    "Test 7: Square with Center Point\n(0,0), (1,0), (1,1), (0,1), (0.5,0.5)",
    "07_square_with_center.png",
)

# Test 8: TwoPoints
print("Generating Test 8: TwoPoints...")
points = np.array([[0.0, 0.0], [1.0, 0.0]])
plot_voronoi(points, "Test 8: Two Points\n(0,0), (1,0)", "08_two_points.png")

# Test 9: HexagonalPattern
print("Generating Test 9: HexagonalPattern...")
points = [[0.0, 0.0]]  # Center
for i in range(6):
    angle = 2.0 * np.pi * i / 6.0
    points.append([np.cos(angle), np.sin(angle)])
points = np.array(points)
plot_voronoi(
    points,
    "Test 9: Hexagonal Pattern\nCenter + 6 points on unit circle",
    "09_hexagonal_pattern.png",
)

# Test 10: RidgeConnectivity (Equilateral Triangle)
print("Generating Test 10: RidgeConnectivity...")
points = np.array([[0.0, 0.0], [1.0, 0.0], [0.5, np.sqrt(3.0) / 2.0]])
plot_voronoi(
    points,
    "Test 10: Equilateral Triangle\n(0,0), (1,0), (0.5, √3/2)\nRidge Connectivity Test",
    "10_equilateral_triangle.png",
)

print(f"\nAll golden test visualizations saved to '{output_dir}/' directory!")
print("Total: 10 test case visualizations generated.")
