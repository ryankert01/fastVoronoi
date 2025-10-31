# Contributing to fastVoronoi

Thank you for your interest in contributing to fastVoronoi! This document provides guidelines for contributing to the project.

## Getting Started

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/fastVoronoi.git
   cd fastVoronoi
   ```
3. Create a new branch for your feature or bugfix:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Development Setup

### Prerequisites

- Python 3.8+
- CMake 3.15+
- C++14 compatible compiler (GCC, Clang, or MSVC)
- Git

### Installation for Development

```bash
# Install with all optional dependencies
pip install -e ".[all]"

# Or install individual components
pip install -e ".[dev]"  # Development tools
pip install -e ".[gis]"  # GeoPandas support
pip install -e ".[viz]"  # Visualization tools
```

## Coding Standards

### Python Code

- Follow PEP 8 style guide
- Use `black` for code formatting:
  ```bash
  black python/ tests/
  ```
- Use type hints where appropriate
- Write docstrings for all public functions and classes

### C++ Code

- Follow C++14 standard
- Use meaningful variable and function names
- Add Doxygen-style comments for public APIs
- Keep functions focused and modular

### Documentation

- Update README.md if you add new features
- Add docstrings to Python code
- Add Doxygen comments to C++ code
- Include examples in `examples/` directory

## Testing

### Running Tests

```bash
# Python tests
pytest tests/test_python.py -v

# C++ tests (if built with -DBUILD_TESTS=ON)
cd build
ctest -V
```

### Writing Tests

- Add test cases for all new features
- Include edge cases and error conditions
- Ensure tests are deterministic
- Use descriptive test names

## Pull Request Process

1. **Update Tests**: Ensure all tests pass and add new tests for your changes
2. **Update Documentation**: Update README.md and docstrings as needed
3. **Commit Messages**: Write clear, concise commit messages
4. **Pull Request**: 
   - Describe what your PR does
   - Reference any related issues
   - Include screenshots for UI/visualization changes

### Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

Example:
```
feat: Add support for weighted Voronoi diagrams

Implement weighted Voronoi diagram computation using
multiplicatively weighted distances.

Closes #123
```

## Reporting Bugs

When reporting bugs, please include:

1. Python and package versions
2. Operating system
3. Minimal code to reproduce the issue
4. Expected vs actual behavior
5. Error messages and stack traces

## Feature Requests

For feature requests:

1. Check if the feature already exists or is planned
2. Describe the use case and benefits
3. Provide example usage if possible
4. Discuss implementation approach

## Code Review

All contributions go through code review. Reviewers will check for:

- Code quality and style
- Test coverage
- Documentation completeness
- Performance considerations
- API design

## License

By contributing, you agree that your contributions will be licensed under the Apache License 2.0.

## Questions?

Feel free to open an issue for questions or discussion!
