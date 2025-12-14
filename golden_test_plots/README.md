# Golden Test Case Visualizations

This directory contains Voronoi diagram visualizations for all 10 golden test cases from the C++ Google Test suite (`tests/cpp/test_voronoi.cpp`).

## Test Cases

1. **01_simple_triangle.png** - Simple triangle with 3 points
2. **02_square_points.png** - Four points forming a square
3. **03_3x3_grid.png** - 3x3 grid from the problem statement
4. **04_collinear_points.png** - Edge case: three collinear points
5. **05_pentagon.png** - Regular pentagon on unit circle
6. **06_deterministic.png** - Deterministic coordinates with known circumcenter
7. **07_square_with_center.png** - Square with center point
8. **08_two_points.png** - Edge case: minimal input (2 points)
9. **09_hexagonal_pattern.png** - Center point with 6 surrounding points in hexagonal pattern
10. **10_equilateral_triangle.png** - Equilateral triangle for ridge connectivity testing

## How to Regenerate

Run the visualization script from the repository root:

```bash
python3 visualize_golden_tests.py
```

The script uses `scipy.spatial.Voronoi` and `voronoi_plot_2d` to generate diagrams that match the expected behavior of our C++ implementation.

## Edge Cases

Tests 4 and 8 (collinear points and two points) are edge cases that cannot be handled by scipy's Voronoi implementation. These are visualized as point plots with a warning message.
