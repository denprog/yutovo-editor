# Yutovo Editor Agent Notes

## Project
Document editor with MathML rendering and solver integration.

## Symbolic Integration
- `ElementType::SYMBOLIC_REAL_RESULT`, `SYMBOLIC_RATIONAL_RESULT`, `SYMBOLIC_COMPLEX_RESULT` added (old `SYMBOLIC_RESULT` removed).
- `Config::AutoResultConfig` now contains `symbolic_real_result` (`RealResultConfig`), `symbolic_rational_result` (`RationalResultConfig`), `symbolic_complex_result` (`ComplexResultConfig`).
- `SymbolicRealResult` / `SymbolicRationalResult` / `SymbolicComplexResult` inherit from `RealResult` / `RationalResult` / `ComplexResult` and override `PutResult` to parse symbolic strings via `AddSymbolicElements` when the result contains alphabetic characters.
- `SymbolicRealSolverTask` / `SymbolicRationalSolverTask` / `SymbolicComplexSolverTask` send JSON with `result_type=SYMBOLIC_REAL/RATIONAL/COMPLEX`.
- `Equation::UpdateResult` **must** handle `ResultType::SYMBOLIC_REAL/RATIONAL/COMPLEX` by creating the corresponding result class; missing this causes tests to hang because no solver task is ever dispatched.
- `ResultTask::Execute` **must** handle `ElementType::SYMBOLIC_REAL/RATIONAL/COMPLEX_RESULT` to deliver the solver response to the corresponding `PutResult`; missing this leaves the waiting symbol (`~`) forever.

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

## Build
Projects are built in their `build/debug/` directories. Use `-j16` maximum for building to avoid OOM kills:
```bash
cd build/debug && cmake ../.. && make -j16 yutovo-editor_tests
```
