"""
Example usage of fortune_voronoi matching SciPy's API
"""

import numpy as np
import fortune_voronoi

# Create the same example from the problem statement
points = np.array([[0, 0], [0, 1], [0, 2], [1, 0], [1, 1], [1, 2], [2, 0], [2, 1], [2, 2]])

# Compute Voronoi diagram
vor = fortune_voronoi.Voronoi(points)

# Print results
print("Voronoi Diagram Computation")
print("=" * 50)
print(f"Input points:\n{vor.points}\n")
print(f"Voronoi vertices:\n{vor.vertices}\n")
print(f"Ridge points (pairs of input points):\n{vor.ridge_points}\n")
print(f"Ridge vertices (pairs of Voronoi vertices):\n{vor.ridge_vertices}\n")
print(f"Regions (vertex indices for each point):\n{vor.regions}\n")
print(f"Point to region mapping:\n{vor.point_region}\n")
print(f"Number of points: {vor.npoints}")
print(f"Dimensions: {vor.ndim}")

# Can also use matplotlib to plot if available
try:
    import matplotlib.pyplot as plt
    from scipy.spatial import voronoi_plot_2d
    from scipy.spatial import Voronoi as ScipyVoronoi

    # Compare with SciPy
    scipy_vor = ScipyVoronoi(points)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

    # Plot SciPy version
    voronoi_plot_2d(
        scipy_vor,
        ax=ax1,
        show_vertices=True,
        line_colors="blue",
        line_width=2,
        line_alpha=0.6,
        point_size=10,
    )
    ax1.set_title("SciPy Voronoi")
    ax1.set_xlim(-0.5, 2.5)
    ax1.set_ylim(-0.5, 2.5)
    ax1.grid(True, alpha=0.3)

    # Plot our version (manually since we don't have voronoi_plot_2d for our type)
    ax2.plot(vor.points[:, 0], vor.points[:, 1], "ko", markersize=10, label="Input points")
    ax2.plot(
        vor.vertices[:, 0],
        vor.vertices[:, 1],
        "r^",
        markersize=8,
        label="Voronoi vertices",
    )

    # Draw ridges
    for ridge in vor.ridge_vertices:
        if ridge[0] >= 0 and ridge[1] >= 0:
            # Finite ridge
            v0 = vor.vertices[ridge[0]]
            v1 = vor.vertices[ridge[1]]
            ax2.plot([v0[0], v1[0]], [v0[1], v1[1]], "b-", linewidth=2, alpha=0.6)

    ax2.set_title("Fortune Voronoi (C++ Implementation)")
    ax2.set_xlim(-0.5, 2.5)
    ax2.set_ylim(-0.5, 2.5)
    ax2.grid(True, alpha=0.3)
    ax2.legend()

    plt.tight_layout()
    plt.savefig("/tmp/voronoi_comparison.png", dpi=150, bbox_inches="tight")
    print("\nPlot saved to /tmp/voronoi_comparison.png")

except ImportError as e:
    print(f"\nSkipping plot (matplotlib not fully available): {e}")
