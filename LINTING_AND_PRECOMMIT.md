# C++ Linting and Pre-commit Hooks

This document describes the C++ linting setup and pre-commit hooks for the Fortune Voronoi library.

## Overview

The project uses multiple tools to maintain code quality:

1. **clang-format** - Automatic C++ code formatting
2. **clang-tidy** - Static analysis for C++ code
3. **pre-commit** - Git hooks for automated checks
4. **Python linters** - black and flake8 for Python code

## C++ Linting Tools

### clang-format

**Purpose**: Automatic code formatting for consistent style

**Configuration**: `.clang-format`

- Based on Google C++ Style Guide
- 4-space indentation
- 100-character line limit
- Organized includes
- Consistent spacing and alignment

**Usage**:

```bash
# Format a single file
clang-format -i src/fortune.cpp

# Format all C++ files
find src include tests/cpp -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Check formatting without modifying (CI)
clang-format --dry-run --Werror src/fortune.cpp
```

**Example**:

```cpp
// Before formatting
void compute_voronoi(const std::vector<std::array<double,2>>& points){
for(int i=0;i<points.size();++i){
// ...
}
}

// After clang-format
void compute_voronoi(const std::vector<std::array<double, 2>>& points) {
    for (int i = 0; i < points.size(); ++i) {
        // ...
    }
}
```

### clang-tidy

**Purpose**: Static analysis for bugs, performance issues, and style violations

**Configuration**: `.clang-tidy`

- Checks: diagnostics, core guidelines, modernization, performance, readability
- Naming conventions enforced
- Modern C++ practices encouraged

**Usage**:

```bash
# Run on a single file
clang-tidy src/fortune.cpp -- -I./include -std=c++17

# Run on all source files
find src -name "*.cpp" | xargs -I{} clang-tidy {} -- -I./include -std=c++17

# Run with build database (recommended)
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy -p . ../src/fortune.cpp
```

**Common Checks**:

- `cppcoreguidelines-*` - C++ Core Guidelines
- `modernize-*` - Modern C++ features (C++11/14/17)
- `performance-*` - Performance improvements
- `readability-*` - Code readability
- `bugprone-*` - Bug detection

**Example Issues**:

```cpp
// Issue: Use auto for type deduction
std::vector<int>::iterator it = vec.begin();  // ❌
auto it = vec.begin();                         // ✅

// Issue: Use const for immutable variables
double x = 1.0;                                // ❌
const double x = 1.0;                          // ✅

// Issue: Pass large objects by const reference
void process(std::vector<double> data);        // ❌
void process(const std::vector<double>& data); // ✅
```

## Pre-commit Hooks

**Purpose**: Automatically check code quality before commits

### Installation

**Prerequisites**:

```bash
# Install Python 3.10 or later
python3 --version

# Install pre-commit
pip install pre-commit
```

**Setup**:

```bash
# Navigate to repository root
cd /path/to/fortune

# Install the git hooks
pre-commit install

# (Optional) Install hooks for commit messages
pre-commit install --hook-type commit-msg
```

### Verification

After installation, pre-commit will run automatically on `git commit`.

**Test the hooks**:

```bash
# Run on all files
pre-commit run --all-files

# Run on staged files only
pre-commit run

# Run a specific hook
pre-commit run clang-format --all-files
```

### Hooks Enabled

1. **clang-format** (C++)
   - Automatically formats C++ code
   - Runs on: `*.cpp`, `*.h`, `*.hpp` files
   - Action: Formats files in-place

2. **black** (Python)
   - Formats Python code
   - Line length: 100 characters
   - Runs on: `*.py` files

3. **flake8** (Python)
   - Lints Python code
   - Max line length: 100
   - Ignores: E203, W503 (black compatibility)

4. **General file checks**
   - Removes trailing whitespace
   - Ensures files end with newline
   - Checks YAML syntax
   - Prevents large files (>1MB)
   - Detects merge conflicts
   - Fixes mixed line endings (LF)

5. **cmake-format**
   - Formats CMakeLists.txt files
   - Ensures consistent CMake style

6. **markdownlint**
   - Checks Markdown files for style issues
   - Excludes: pybind11/, build/

### Workflow

#### Normal Development

```bash
# 1. Make changes to code
vim src/fortune.cpp

# 2. Stage changes
git add src/fortune.cpp

# 3. Commit (pre-commit runs automatically)
git commit -m "Add new feature"

# If hooks fail, fix issues and re-commit
# Hooks will auto-fix many issues (formatting)
git add src/fortune.cpp  # Stage auto-fixes
git commit -m "Add new feature"
```

#### Skipping Hooks (Emergency Only)

```bash
# Skip all pre-commit hooks (NOT RECOMMENDED)
git commit --no-verify -m "Emergency fix"
```

⚠️ **Warning**: Skipping hooks may introduce code quality issues and CI failures.

#### Manual Formatting

```bash
# Format C++ code manually
pre-commit run clang-format --all-files

# Format Python code manually
pre-commit run black --all-files

# Run all hooks manually
pre-commit run --all-files
```

### CI Integration

Pre-commit hooks are also run in CI:

```yaml
# .github/workflows/ci.yml
- name: Run pre-commit
  run: |
    pip install pre-commit
    pre-commit run --all-files
```

This ensures code pushed to GitHub passes all quality checks.

## Configuration Files

### .clang-format

```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
PointerAlignment: Left
# ... (see file for full config)
```

Key settings:

- 4-space indentation
- 100-character line limit
- Google style base with modifications
- Left-aligned pointers (`int* ptr`)
- Sorted includes

### .clang-tidy

```yaml
Checks: >
  clang-diagnostic-*,
  clang-analyzer-*,
  cppcoreguidelines-*,
  modernize-*,
  performance-*,
  readability-*,
  bugprone-*
  # ... (see file for exclusions)
```

Key checks:

- Core C++ guidelines compliance
- Modern C++ feature usage
- Performance optimizations
- Code readability improvements
- Bug detection

### .pre-commit-config.yaml

```yaml
repos:
  - repo: https://github.com/pre-commit/mirrors-clang-format
    rev: v19.1.5
    hooks:
      - id: clang-format
  # ... (see file for all hooks)
```

## Updating Pre-commit Hooks

```bash
# Update to latest versions
pre-commit autoupdate

# This updates hook versions in .pre-commit-config.yaml
# Commit the changes
git add .pre-commit-config.yaml
git commit -m "Update pre-commit hooks"
```

## Troubleshooting

### Pre-commit not running

```bash
# Reinstall hooks
pre-commit uninstall
pre-commit install

# Verify installation
pre-commit run --all-files
```

### clang-format not found

```bash
# Install clang-format (Ubuntu/Debian)
sudo apt-get install clang-format

# Install clang-format (macOS)
brew install clang-format

# Verify installation
clang-format --version
```

### clang-tidy errors

```bash
# Generate compile_commands.json for better analysis
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Run clang-tidy with build database
clang-tidy -p . ../src/fortune.cpp
```

### Hook taking too long

```bash
# Skip slow hooks for quick commits
SKIP=cmake-lint,markdownlint git commit -m "Quick fix"

# Or disable specific hooks in .pre-commit-config.yaml
```

### Conflicts with existing code

```bash
# Format all existing code once
pre-commit run --all-files

# Commit formatting changes separately
git add -A
git commit -m "Apply code formatting"
```

## Best Practices

### For Contributors

1. **Install pre-commit hooks immediately** after cloning

   ```bash
   git clone https://github.com/ryankert01/fortune.git
   cd fortune
   pip install pre-commit
   pre-commit install
   ```

2. **Run hooks before pushing**

   ```bash
   pre-commit run --all-files
   ```

3. **Keep hooks updated**

   ```bash
   pre-commit autoupdate  # Monthly
   ```

4. **Don't skip hooks** unless absolutely necessary

5. **Commit formatting separately** from logic changes

### For Maintainers

1. **Enforce hooks in CI** to catch issues before merge

2. **Update hook versions** regularly

3. **Review .clang-tidy warnings** and adjust rules as needed

4. **Document exceptions** when disabling specific checks

5. **Monitor hook performance** and optimize if needed

## Examples

### Example: Adding a New Feature

```bash
# 1. Create feature branch
git checkout -b feature/new-algorithm

# 2. Write code
vim src/fortune.cpp

# 3. Let pre-commit format it
git add src/fortune.cpp
git commit -m "Add new algorithm"
# Pre-commit runs automatically, may modify files

# 4. If files were modified, stage and recommit
git add src/fortune.cpp
git commit -m "Add new algorithm"

# 5. Push to GitHub
git push origin feature/new-algorithm
```

### Example: Fixing Style Issues

```bash
# Format all C++ files
pre-commit run clang-format --all-files

# Check for clang-tidy issues
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy -p . ../src/fortune.cpp

# Commit style fixes
git add -A
git commit -m "Fix code style issues"
```

## Integration with IDEs

### Visual Studio Code

Install extensions:

- **C/C++** by Microsoft
- **clang-format** by xaver

Settings (`.vscode/settings.json`):

```json
{
    "C_Cpp.clang_format_style": "file",
    "editor.formatOnSave": true,
    "python.formatting.provider": "black",
    "python.linting.flake8Enabled": true
}
```

### CLion/IntelliJ

Settings → Editor → Code Style → C/C++:

- Enable "ClangFormat"
- Select "Use .clang-format file"

## Summary

| Tool | Purpose | Config File | Auto-fix |
|------|---------|-------------|----------|
| clang-format | C++ formatting | `.clang-format` | ✅ Yes |
| clang-tidy | C++ static analysis | `.clang-tidy` | ⚠️ Some |
| pre-commit | Git hooks | `.pre-commit-config.yaml` | ✅ Yes |
| black | Python formatting | Inline | ✅ Yes |
| flake8 | Python linting | Inline | ❌ No |

**Benefits**:

- ✅ Consistent code style across all contributors
- ✅ Catch bugs early with static analysis
- ✅ Automatic formatting saves time
- ✅ CI integration prevents bad code from merging
- ✅ Better code reviews (focus on logic, not style)

**Quick Start**:

```bash
# Install and enable
pip install pre-commit
pre-commit install

# Format existing code
pre-commit run --all-files

# Normal development
git add .
git commit -m "Your changes"  # Hooks run automatically
```

For questions or issues, see the main README.md or open a GitHub issue.
