# Yutovo Editor Agent Notes

## Project
Document editor with MathML rendering and solver integration.

## Agent Rules

- **Never delete files without explicit user permission.** Do not remove source files, test files, core dumps, logs, build artifacts, or any other files unless the user explicitly asks for it. When in doubt, leave the file in place and ask.
- **Never create separate namespaces (such as `namespace detail` or anonymous namespaces) without explicit user permission.** Helper functions should be placed in the common `yutovo_calculator` namespace, for example in `utils.h/utils.cpp` or `giac_utils.h/giac_utils.cpp`, or as `static` methods of the appropriate class.
- **Never commit without explicit user permission.** Do not run `git commit`, `git push`, `git reset`, `git rebase`, or any other git mutations unless explicitly asked to do so. Ask for confirmation each time when git mutations are needed.
- **Never delete existing tests.** When fixing regressions or refactoring, update test expectations to match the new correct behavior, but do not remove tests. If `git checkout` or similar commands are used to revert a file, verify that no user-added tests were lost.

## Symbolic Integration
- `ElementType::SYMBOLIC_REAL_RESULT`, `SYMBOLIC_RATIONAL_RESULT`, `SYMBOLIC_COMPLEX_RESULT` added (old `SYMBOLIC_RESULT` removed).
- `Config::AutoResultConfig` now contains `symbolic_real_result` (`RealResultConfig`), `symbolic_rational_result` (`RationalResultConfig`), `symbolic_complex_result` (`ComplexResultConfig`).
- `SymbolicRealResult` / `SymbolicRationalResult` / `SymbolicComplexResult` inherit from `RealResult` / `RationalResult` / `ComplexResult` and override `PutResult` to parse symbolic strings via `AddSymbolicElements` when the result contains alphabetic characters.
- `SymbolicRealSolverTask` / `SymbolicRationalSolverTask` / `SymbolicComplexSolverTask` send JSON with `result_type=SYMBOLIC_REAL/RATIONAL/COMPLEX`.
- `Equation::UpdateResult` **must** handle `ResultType::SYMBOLIC_REAL/RATIONAL/COMPLEX` by creating the corresponding result class; missing this causes tests to hang because no solver task is ever dispatched.
- `ResultTask::Execute` **must** handle `ElementType::SYMBOLIC_REAL/RATIONAL/COMPLEX_RESULT` to deliver the solver response to the corresponding `PutResult`; missing this leaves the waiting symbol (`~`) forever.

## Definite Integral Element
- `ElementType::DEFINITE_INTEGRAL`, class `DefiniteIntegral : public Formula` in `src/formulas/definite_integral.h/.cpp` (definite integral, symbol `∫`).
- Children: `CodeRow` lower limit (0), `Shape` integral symbol (1), `CodeRow` upper limit (2), `CodeRow` integrand (3), non-editable `CodeString` "d" (4, `editable = false`), `CodeRow` integration variable (5).
- `ToText()` and `ToParserString()` produce `definite_integral(lower,upper,integrand,var)` — all 4 editable rows are passed to the calculator through `definite_integral()`.
- Insert via `document.InsertDefiniteIntegral(with_undo)`; undo stores/restores children 0, 2, 3, 5 (see undo.cpp); registered in editor_utils.cpp `create_elements`.
- Tests: `test/definite_integral.cpp` (`FormulaTest.definite_integral1..definite_integral9`).

## Indefinite Integral Element
- `ElementType::INDEFINITE_INTEGRAL`, class `IndefiniteIntegral : public Formula` in `src/formulas/indefinite_integral.h/.cpp` (indefinite integral, symbol `∫`).
- Children: `Shape` integral symbol (0), `CodeRow` integrand (1), non-editable `CodeString` "d" (2, `editable = false`), `CodeRow` integration variable (3).
- `ToText()` and `ToParserString()` produce `indefinite_integral(integrand,var)` — the 2 editable rows are passed to the calculator through `indefinite_integral()`.
- Insert via `document.InsertIndefiniteIntegral(with_undo)`; undo stores/restores children 1, 3 (see undo.cpp); registered in editor_utils.cpp `create_elements`.
- Tests: `test/indefinite_integral.cpp` (`FormulaTest.indefinite_integral1..indefinite_integral9`).

## Code Style

### Parenthesized expressions
Keep the contents of parentheses (function argument lists, conditions, initializers, etc.) on a single line when it fits. Only wrap to a new line if the expression would exceed **140 columns**.
When a parenthesized expression is wrapped, each continuation line uses the normal **4-space indent**; do not align arguments with the opening parenthesis.
```cpp
// CORRECT
void ShortFunction(int a, int b, int c);

void LongFunctionName(const std::u32string& first_argument, const std::u32string& second_argument,
    int third_argument);

auto result = SomeFunction(first_argument, second_argument,
    third_argument, fourth_argument);

if (condition_a && condition_b)
{
    // ...
}

// WRONG
void LongFunctionName(
    const std::u32string& first_argument,
    const std::u32string& second_argument,
    int third_argument);

void LongFunctionName(const std::u32string& first_argument,
                      const std::u32string& second_argument,
                      int third_argument);

try {
    // ...
} catch (...) {
    // ...
}
```

### Spaces around brackets
Do not put spaces before or after square brackets `[]` and round brackets `()`:
```cpp
// CORRECT
int arr[10];
void foo(int a);
arr[0] = foo(1);

// WRONG
int arr [10];
void foo (int a);
arr [0] = foo (1);
```

## Editor Test Patterns

### Entering expressions in code blocks
Do **not** put operators inside `InsertString`:
```cpp
// WRONG
 document.InsertString("x+1", true);

// CORRECT
 document.InsertString("x", true);
 document.InsertPlus(true);
 document.InsertString("1", true);
```

Use dedicated insert methods for operators:
- `InsertPlus(true)` / `InsertMinus(true)`
- `InsertMultiply(true)` / `InsertDivision(true)`
- `InsertPower(true)` — wraps the preceding string element into the base automatically; caret moves to exponent.
- `InsertOpenRoundBracket(true)` / `InsertCloseRoundBracket(true)`
- `InsertComma(true)`

### Power behavior
`InsertPower(true)` after a string moves that string into the base and places the caret in the exponent.
Example:
```cpp
document.InsertString("x", true);
document.InsertPower(true);
document.InsertString("2", true);
// Parser text: pow(x,2)
```

### Checking results with ToText()
For AUTO-mode symbolic fallback tests, `ToText()` is sufficient:
```cpp
ASSERT_TRUE(document.ToText() == U"x+1=1+x") << ToBasicString(document.ToText());
```

### Checking results with ToHtml()
Check full `ToHtml()` exactly like `SolverSymbolicTest::solver1`:
```cpp
ASSERT_TRUE(document.ToHtml() ==
    "<body>"
        "<p>"
            "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                "<mrow>"
                    "<mrow>"
                        "<mi>x</mi>"
                        "<mo>+</mo>"
                        "<mi>1</mi>"
                    "</mrow>"
                    "<mo>=</mo>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>1</mi>"
                            "<mo>+</mo>"
                            "<mi>x</mi>"
                        "</mrow>"
                    "</mrow>"
                "</mrow>"
            "</math>"
        "</p>"
    "</body>") << document.ToHtml();
```

HTML structure:
- `CodeParagraph` → `<math xmlns='http://www.w3.org/1998/Math/MathML'>`
- `CodeRow` → `<mrow>`
- `CodeString` → `<mi>text</mi>`
- `Plus` / `Minus` → `<mo>+</mo>` / `<mo>-</mo>`
- `Multiply` → `<mo>×</mo>` (not `<mo>*</mo>`)
- `Power` → `<msup><mrow>base</mrow><mrow>exp</mrow></msup>`
- `SymbolicResult` (inside last `CodeRow`) produces `<mrow><mrow>elements...</mrow></mrow>` because `ResultRow` (`CodeColumn`) wraps its `CodeRow` content.

### Undo checks
If the test calls `Undo()`, always assert the document state after undo:
```cpp
document.Undo();
document.WaitUndo();
std::this_thread::sleep_for(200ms);
ASSERT_TRUE(document.ToHtml() == "<body>...</body>") << document.ToHtml();
```

### Async operations and timeouts
Editor and caret methods return a task id and run asynchronously. Always wait for them with `document.WaitTask(id, timeout)` and assert the returned value; do not rely only on `std::this_thread::sleep_for`.

### Changing config
Do not mutate `document.config` directly and then call `document.SetConfig(document.config, false)` — `SetConfigTask` compares the passed config with the current `document.config`, so a direct mutation makes the comparison see no change. Instead, copy `document.config` into a local `Config`, modify the copy, and pass it to `SetConfig`.

## File formats
- `.yut` files are ZIP archives (not plain text). Use `unzip -l file.yut` to list contents, `unzip -p file.yut` to extract.

## Build
Projects are built and tested in `build/debug/`:
```bash
cd build/debug && cmake ../.. && make -j16 yutovo-editor_tests && ./test/yutovo-editor_tests --gtest_filter="FormulaTest.power23"
```

## Testing

### Full test suite
Before running the full `yutovo-editor_tests` suite, ask the user at the beginning of the task (after planning) whether to run the complete suite or only targeted tests.

### Debug giac linkage
`yutovo-editor` creates its own imported `giac_imported` target in `src/CMakeLists.txt`. Debug builds must use `${INSTALL_PATH}/lib/libgiacd.a` and release builds `${INSTALL_PATH}/lib/libgiac.a`; linking a debug `yutovo-calculator`/`yutovo-solver` against the release giac library causes an ABI mismatch and memory corruption inside giac (e.g., `CodeTest.code35` crashing in `giac::expand`).

## Network Errors
If an operation fails with a "Network connection failed" error:
1. Wait **2 seconds** and retry automatically on your own.
2. If the retry still fails, continue retrying with a **10-second** interval.
