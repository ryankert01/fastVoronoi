"""
Performance benchmark for Voronoi diagram computation
"""

import numpy as np
import time
import fastvoronoi

def benchmark_build(n_points):
    """Benchmark Voronoi diagram construction."""
    # Generate random points
    np.random.seed(42)
    points = np.random.rand(n_points, 2) * 100
    
    # Time the construction
    start = time.time()
    voronoi = fastvoronoi.Voronoi(points)
    build_time = time.time() - start
    
    return build_time, voronoi

def benchmark_query(voronoi, n_queries):
    """Benchmark nearest neighbor queries."""
    # Generate random query points
    np.random.seed(43)
    queries = np.random.rand(n_queries, 2) * 100
    
    # Time the queries
    start = time.time()
    nearest = voronoi.find_nearest(queries)
    query_time = time.time() - start
    
    return query_time

if __name__ == '__main__':
    print("fastVoronoi Performance Benchmark")
    print("=" * 50)
    
    # Test different sizes
    sizes = [10, 50, 100, 500, 1000, 5000]
    
    print("\nBuild Performance:")
    print(f"{'Points':<10} {'Time (s)':<12} {'Points/sec':<12}")
    print("-" * 50)
    
    results = {}
    for n in sizes:
        build_time, voronoi = benchmark_build(n)
        points_per_sec = n / build_time if build_time > 0 else 0
        results[n] = voronoi
        print(f"{n:<10} {build_time:<12.6f} {points_per_sec:<12.0f}")
    
    print("\nQuery Performance (1000 queries):")
    print(f"{'Points':<10} {'Time (s)':<12} {'Queries/sec':<12}")
    print("-" * 50)
    
    n_queries = 1000
    for n in sizes:
        if n in results:
            query_time = benchmark_query(results[n], n_queries)
            queries_per_sec = n_queries / query_time if query_time > 0 else 0
            print(f"{n:<10} {query_time:<12.6f} {queries_per_sec:<12.0f}")
    
    print("\nBenchmark complete!")
