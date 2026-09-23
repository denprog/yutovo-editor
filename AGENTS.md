# Yutovo Editor Agent Notes

## Project
Document editor with MathML rendering and solver integration.

## Agent Rules

- **Use `repowise` for codebase navigation and search.** Prefer `mcp__repowise__get_answer`, `mcp__repowise__get_context`, `mcp__repowise__search_codebase`, and related tools over manual `Read`/`Grep` when exploring unfamiliar code, locating symbols, or analyzing architecture.
- **Never delete files without explicit user permission.** Do not remove source files, test files, core dumps, logs, build artifacts, or any other files unless the user explicitly asks for it. When in doubt, leave the file in place and ask.
- **Never create separate namespaces (such as `namespace detail` or anonymous namespaces) without explicit user permission.** Helper functions should be placed in the common `yutovo_calculator` namespace, for example in `utils.h/utils.cpp` or `giac_utils.h/giac_utils.cpp`, or as `static` methods of the appropriate class.
- **Never commit without explicit user permission.** Do not run `git commit`, `git push`, `git reset`, `git rebase`, or any other git mutations unless explicitly asked to do so. Ask for confirmation each time when git mutations are needed.
- **Never delete existing tests.** When fixing regressions or refactoring, update test expectations to match the new correct behavior, but do not remove tests. If `git checkout` or similar commands are used to revert a file, verify that no user-added tests were lost.
- **Add new `ElementType` values only at the end of the enum.** `editor_utils.h` serializes element types as integers, so inserting values in the middle changes the numeric IDs of existing types and breaks saved documents.

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

## Derivative Element

Derivatives are represented by a regular editable `Division` fraction so that the `d`/`∂` prefixes, the function, and the variables are all editable.

- A derivative fraction has the form `d f / d x` (or `∂ f / ∂ x` for partial derivatives). For order `n > 1` the numerator starts with `pow(d, n)` / `pow(∂, n)`.
- `Division::BuildDerivativeParserString()` detects the derivative pattern in `ToText()`/`ToParserString()` and emits nested `derivative(...)` calls. The total differentiation order in the numerator must equal the sum of differentiation operators in the denominator; otherwise the fraction falls back to plain `(num)/(den)`.
  - `d f / d x` → `derivative(f,x)`
  - `pow(d,2) f / pow(d x, 2)` → `derivative(derivative(f,x),x)` (single-variable higher-order)
  - `pow(d,2) f / d x d y` → `derivative(derivative(f,y),x)` (rightmost denominator variable is the innermost derivative)
  - `pow(d,3) f(x,y,z) / d x d y d z` → `derivative(derivative(derivative(f(x,y,z),z),y),x)`
  - `∂ f / ∂ x` → `derivative(f,x)` (partial)
- The total differentiation order is the sum of operators in the denominator (`d`/`∂` = 1, `pow(d,n)`/`pow(∂,n)` = `n`). If it does not equal the numerator order, the fraction falls back to ordinary `(num)/(den)` output.
- The parser recognizes the derivative marker even when it is merged with the function or variable in a single `CodeString` (e.g. `dg(x,y)`/`dxdy`), because `BuildDerivativeParserString()` scans the text character-by-character.
- The `d`/`∂` prefix strings and the empty function/variable placeholders are created with `can_merge = false` so they remain distinct editable elements (see `String::Merge`).
- Insert via `document.InsertDerivative`, `InsertSecondDerivative`, `InsertPartialDerivative`, `InsertPartialSecondDerivative` (all implemented with `CreateDerivativeDivision()` in `document.cpp`).
- Undo/redo stores/restores the editable `CodeString` children inside the `Division` (see `undo.cpp`).
- Tests: `test/derivative.cpp` (`FormulaTest.derivative1..derivative17`) and `test/solver_derivative.cpp` (`SolverAutoTest.derivative1/derivative2/derivative_second1/derivative_third1/derivative_mixed_func/derivative_mixed_func_g/derivative_tan`, `SolverSymbolicTest.derivative1`). Derivative tests that target an explicit numeric result type (e.g. `ResultType::REAL`) belong in the matching `test/solver_<type>.cpp` file (e.g. `SolverRealTest.derivative_mixed_func_g` and `SolverRealTest.derivative_mixed_func_g_real` in `test/solver_real.cpp`).
- Test helpers for building derivative `Division` instances live as methods of `SolverTest` (`test/mock.h`), not as file-scope `static` functions. The helper **must emulate user input** (`InsertDerivative`, `InsertString`, `InsertPower`, `MoveCaretDown`, etc.) instead of constructing `Division`/`CodeString`/`Power` objects by hand, because the desktop insertion/remake path differs from direct element construction.
- Trigonometric/hyperbolic aliases (`tg`/`tan`, `ctg`/`cot`, `cosec`/`csc`, `sh`/`sinh`, `ch`/`cosh`, `th`/`tanh`, `cth`/`coth`, `sch`/`sech`, `cosech`/`csch`) and inverse trig aliases (`arctg`/`arctan`, `arcctg`/`arccot`, `arccosec`/`arccsc`) are registered in all three symbolic parsers. Canonical symbolic function names are `tg`, `arctg`, `arcctg` (renamed from `tan`/`arctan`/`arccot`).
- TermDegree `known_funcs` in `yutovo-calculator/src/symbolic.h` and `FunctionSortRank` in `yutovo-calculator/src/giac_utils.cpp` include all aliases and canonical output names (`tan`, `asin`, `acos`, `atan`, etc.).
- `CalcTestSymbolicReal.all_symbolic_functions` covers every symbolic trig/hyperbolic/inverse function with four variants: numeric evaluation, symbolic form, derivative, and alias equivalence.

## Evaluation at a Point
- Evaluating an expression at a point `expr |_{x=2}` reuses the existing `EvaluationBarSubscript` (evaluation bar with assignments in the subscript) — no new element types. The bar is inserted with `document.InsertEvaluationBarSubscript(with_undo)`; users type the expression before it and fill the `x=2` rows in the subscript.
- `Document::InsertFunctionAtPoint(with_undo)` (document.cpp, after `InsertDerivativeAtPoint`) inserts the template: the evaluation bar, then round brackets `( )` before it (same pattern as `InsertRoundBrackets`: `InsertFormulas(..., select_pos=1)` + one `MoveCaretLeft`) — the caret lands between the brackets; four `MoveCaretRight` presses get into the first bar assignment row, two more get out of the bar after filling the value.
- `CodeRow::ToParserString` (src/formulas/code_row.cpp) wraps the segment before such a bar into `evaluate(expr,[x=2,...])`. The bar is skipped when it directly follows a derivative fraction (`Division::BuildDerivativeAtPointParserString` consumes it into `derivative(f,[x=2])`), or when the assignments or the expression are empty (soft degradation while editing). `ToText()` stays the plain concatenation `expr[x=2]`.
- yutovo-calculator parses `evaluate(expr,[x=2,y=3])` in all six parsers (grammar rule `evaluate_at_point`, AST node `EvaluateAtPointNode`, first alternative of `unary`): numeric types evaluate via `Solver::EvaluateWithTempVariables` (temp variables), symbolic types parse the expression string and apply `subs` per variable. `"evaluate"` is in TermDegree `known_funcs` (symbolic.h).
- Tests: `test/evaluation_bar.cpp` (`FormulaTest.evaluate1`: editing, full `ToHtml()`, caret, undo/redo; `FormulaTest.function_at_point1`: the inserted template `( ) |`, filling it, full `ToHtml()`, caret, undo/redo), `test/solver_evaluate.cpp` (`SolverAutoTest.evaluate1..4`: value, multi variable, user function, bar after a plain fraction → `(x)/(y)[y=2,x=6]=3.`; `SolverAutoTest.function_at_point1`: template + solve → `(pow(x,2))[x=3]=9.`), calculator tests `evaluate*` in all six calculator test files.
- Front-ends: the "Evaluate at point" button inserts the bar — desktop `actionEvaluationBar` (algebra toolbar, `MainWindow::OnEvaluationBar`, test `TestToolbar::testEvaluationBar`), web `onEvalutionBar()` in `YutovoWeb.vue` must `Module.cwrap('OnEvaluationBar', ...)` (the wasm export in `src/main.cpp` is spelled without the typo). The "Function at point" button inserts the template — desktop `actionFunctionAtPoint` (calculus toolbar after "Derivative at point", icon `images/algebra/function_at_point.png`, test `TestToolbar::testFunctionAtPoint`), web `onFunctionAtPoint()` → `OnFunctionAtPoint` (`test/function_at_point.spec.js`).

## Rebuild and Deploy
- Projects link prebuilt libraries from `$YUTOVO_DEPLOY`, and `make` does not track imported `.a` changes: after `make install` of yutovo-calculator/yutovo-solver/yutovo-editor, delete the dependant build artifacts to force relinking (`rm` the executables, or `find . -name "*.a" -delete` inside their build dir, then rebuild). A stale link shows up as `Syntax error` results for new syntax.
- Editor tests use the builtin solver through a separate worker process (`yutovo-solver-calculator-workerd`): `FindWorkerExecutable` looks next to the test binary first, then `$YUTOVO_DEPLOY/bin`, then the working directory. Stale workerd copies (e.g. `build/debug/yutovo-solver-calculator-workerd`) keep parsing with the old calculator after changes.

## Text Block
- `ElementType::TEXT_BLOCK`, `TEXT_EQUATION`, `TEXT_ASSIGNMENT` (appended at the end of the enum). Classes `TextBlock : public Block` (`src/formulas/text_block.h/.cpp`), `TextEquation : public Equation` (`src/formulas/text_equation.h/.cpp`), `TextAssignment : public Assignment` (`src/formulas/text_assignment.h/.cpp`).
- TextBlock is a block for entering formulas **without solving**: editing works exactly like in CodeBlock (CodeParagraph/CodeRow/CodeString children, same "Calculator"/"Code"/"Formula" formats), but nothing is computed. It can be inserted into document text via `document.InsertTextBlock(with_undo)` and inside a CodeBlock (stays a nested block). Neither a CodeBlock nor another TextBlock can be inserted inside a TextBlock — `CodeRow::InsertElements` rejects both.
- `TextBlock::AfterFromJson` converts plain `STRING` elements saved by older versions into `CODE_STRING` (`TextBlock::ConvertStringsToCode`, which must not descend into `STRING`/`CODE_STRING`/`LINK` - their element lists return the element itself).
- `TextEquation`/`TextAssignment` never solve: `TextEquation` overrides `Solve`/`ReSolve` with empty bodies, `TextAssignment` sets `auto_solve = false`. The right part stays a plain editable CodeRow, and `TextEquation::AfterInsert` puts the caret into the right row (typing on the `=` shape is rejected by `MiddleShapeFormula::InsertElements`). `TextAssignment` uses the plain `:` sign (`solve_sign`/`draw_sign`, ToText `x:5`), unlike the `:=` of a computing Assignment.
- Typing `=`/`:` inside a TextBlock: `InsertFormulasTask::Execute` converts the cloned `EQUATION`/`ASSIGNMENT` elements into their text counterparts via `TextBlock::ConvertToText`, so front-ends calling `InsertEquation`/`InsertAssignment` work unchanged. Pasting a whole CodeBlock into a TextBlock flattens its paragraphs and converts them the same way (`CodeRow::InsertElements` calls `TextBlock::ConvertToText` on the collected rows when `TextBlock::BlocksSolving(this)`); a solved result row is flattened into a plain editable row by `TextBlock::CollectRowElements`, which recurses through nested rows **and results** (an `AutoResult` holds a `RealResult` child after solving - without the recursion the result element would be pasted as-is and lost on save). An empty code block is still rejected.
- `Config::TextBlockConfig text_block` contains `background_color` (default `#cfcab0`, yellowish) and `frame_color` (default White). Like the other colors these are **application settings** - they are not serialized into the document config (`Config::ToJson`/`FromJson` must not write them, otherwise a saved document would override the application colors on load). The frame is gated by the shared `code_block_border` flag; other visual parameters come from the CodeBlock formats. yutovo-desktop persists both colors in QSettings as `text_block_background_color`/`text_block_frame_color`.
- Undo/redo: `TEXT_EQUATION`/`TEXT_ASSIGNMENT` are stored like `ASSIGNMENT` (children 0 and 2 in `UndoFormula`), `TEXT_BLOCK` has `UndoTextBlock` (formats + children, see `undo.cpp`).
- Tests: `test/text_block.cpp` (`TextBlockTest.text_block1..15`, `text_block15` copies a code block with an assignment and a solved equation and pastes it into a text block, `text_block12`/`text_block13` check that `InsertCode`/`InsertTextBlock` inside a text block are rejected, `text_block14` loads a handwritten json with a legacy plain `STRING` in the right part and checks it becomes a code string); `text_block10` checks that a text block inserted into document text accepts formulas after `=` (exact `ToHtml()` MathML with an `mfrac`), `text_block11` saves to a `.yut` file and checks the stored json (a gzip stream unpacked via `Boost::iostreams`, which the test target already links). Do **not** use `FindAllByType` from `mock.h` on documents containing strings — `StringElements::Get(pos)` returns the string itself, causing infinite recursion; count elements with a helper that skips `STRING`/`CODE_STRING`.
- Front-ends: formula commands ("=", ":", "+", ...) must be allowed inside a text block placed in the document **text** too — `CommandContext::Formula` in both `command_map.cpp` files (desktop and web) accepts `TEXT_BLOCK` as well as `CODE_BLOCK` ancestors, otherwise "=" is typed as a plain string. yutovo-desktop has a toolbar button next to "Insert calculator" (`text_block_action`, `MainWindow::OnInsertTextBlock` in `mainwindow.cpp`, icon `images/format/text_block.png`); yutovo-web has `#insert-text-block-button` (`YutovoWeb.vue` → wasm export `OnInsertTextBlock` in `src/main.cpp`). Both link the prebuilt editor from `$YUTOVO_DEPLOY` — reinstall yutovo-editor (native and wasm builds) into deploy before rebuilding them. Button tests: desktop `TestToolbar::testTextBlock`, `TestToolbar::testTextBlockInText` (keyboard input into a block placed in the document text, clicks via `QTest::mouseClick`), (`test/toolbar.cpp`, the action is found by objectName `text_block_action`; run the whole `yutovo-desktop_test` with a real display — `TestFiles::testCopyPasteGraph` aborts offscreen), web `test/text_block.spec.js` (`utils.insertTextBlock`; click the button only after the page settled — a click right after `setLanguage` misses while the language list is closing).

## 3D Surface Graph
- `ElementType::GRAPH_SURFACE` (appended at the end of the enum). Class `GraphSurface : public Graph` lives in `src/formulas/graph.h/.cpp` next to `GraphLine` and reuses the `Graph` base (ranges, `SetNumber`, MathGL instance, solving/error state).
- Children (in caret traversal order): 0 y_top `CodeRow`, 1 expressions `CodeParagraphsBlock` (one paragraph = one surface, like plots in `GraphLine`), 2 y variable, 3 y_bottom, 4 x_left, 5 x variable, 6 x_right, 7 `Shape` (`can_resize`/`can_move_picture`). Layout: left column top→bottom = y_top, [expressions block | y variable side by side at mid height], y_bottom; bottom row = x_left, x variable (center), x_right. The y variable row sits **right of the expressions block** at the middle of the height. The 2D getters from `Graph` use different indices, so `GraphSurface` shadows `GetYVariable`/`GetYBottom`/`GetXLeft`/`GetVariable`/`GetXRight`/`GetShape` and overrides `Init`/`Remake`/`MovePicture`/`ZoomPicture`/`UpdateLevel`. **`ZoomPicture` must use the surface getters** — the base version writes the bound rows by the 2D child indices. `AfterFromJson` validates the restored child **types** (CODE_ROW / CODE_PARAGRAPHS_BLOCK / CODE_ROW×5 / SHAPE) and returns false on a mismatch - a wrong structure turns into a clean load error (`document not parsed`), never a crash. Do not try to reorder children with `Elements::Remove`/`Insert` during load - `Remove` resolves the position by id and corrupts the tree when the ids do not match. `ToText` keeps the public order `graph_surface(y_top, expr, y_bottom, x_left, x_var, x_right, y_var)` regardless of the child order (it is also called while `Init` builds the children - guard `Count() < 8`).
- Rendering (MathGL): `graph.Rotate(rot_x, rot_z)` (defaults 50/60, stored in the element and serialized as `rot_x`/`rot_z`), z-range computed from the finite samples, `SetRanges(x_left, x_right, y_bottom, y_top, z_min, z_max)`, `Box` + `Axis("xyz")` + `Grid("xyz")`, then per plot by `PlotFormat::style`: `SurfaceStyle::HEIGHT` → `Surf(z, "")` (gradient by height), `UNIFORM` → `Surf(z, "{xRRGGBB}")`, `HEIGHT_MESH` → `Surf(z, "#")`, `WIREFRAME` → `Mesh(z, "{xRRGGBB}-<width>")`, `POINTS` → `Surf(z, "{xRRGGBB}.<width clamped to 1..9>")` — the point size comes from the pen width digit of the scheme (`mglCanvas::mark_plot`: `type=='.'` uses `PenWidth`, not `MarkSize`). `Color::ToRGB()` already returns the MathGL `"xRRGGBB"` form. The grid is `mglData::Set(z.data(), nx, ny)` — x index fastest, matching the calculator layout.
- Mouse: drag **rotates** the view (`MovePicture(dx, dy, shift=false)` changes the angles, redraw only, no re-solve; the vertical rotation is inverted so the surface follows the mouse - down means tilt down); **Shift+drag** pans the x/y ranges so the surface follows the cursor by the mouse distance (rewrites the bound rows via `SetNumber`, re-solves); wheel zooms (`GraphSurface::ZoomPicture` override). The shift flag is plumbed through `Element::MovePicture(dx, dy, bool shift=false)` → `MovePictureTask` → `Document::MouseMove(x, y, shift=false)` → front-ends (desktop `mouseMoveEvent` passes `Qt::ShiftModifier`, web `OnMouseMove` passes `mouse_event->shiftKey`).
- The mouse tasks (`MovePictureTask`, `ZoomPictureTask`, `ResizeElementTask`) have no undo, but they change saved content (ranges, `rot_x`/`rot_z`, `format.size`) - each of them calls `Document::SetLastModifyTaskId(Task::id)` after executing so `UpdateChanged()` marks the document modified (`Task::id` must be qualified because these tasks shadow `id` with their `ElementId id` member). `Document::UpdateChanged` must NOT force `changed = false` when `save_task_id == 0 && undo_tasks.empty()` - a **loaded** document keeps exactly that state, and the forced branch silently swallowed every non-undoable modification of it.
- The 3D Shift-pan converts the pixel delta into data units through the **current projection** (`GraphSurface::MovePicture`): it draws the surfaces into the MathGL frame, finalizes it with `GetRGBA()` (the z buffer is only filled during rasterization), samples `CalcXYZ` at the plot center and ±10px around it, and combines the samples linearly. A bare `CalcXYZ` without a drawn z buffer unprojects onto the most visible coordinate **plane** (the x component is identically 0 at the default rotation) - that is why the frame must be drawn first.
- Data protocol: no new `ResultType` — the graph solves as `ARRAY_REAL`. `GraphSurface::Solve()` sends `graph_surface(expr, x_var, y_var, x_left, x_right, y_bottom, y_top, nx, ny)` per expression paragraph, nx/ny = clamp(shape_size/8, 10..80). In `PutResult` values[0..3] are the bounds, values[4]/[5] are nx/ny, the rest are z values row-major (x fastest); failed points are NaN (holes in the surface).
- Soft degradation while editing: `Solve()` **does not dispatch** while any of the six bound/variable rows or any expression paragraph is empty (the incomplete `graph_surface(...)` string would be a parser error); it resets plots/error state and shows the empty frame instead. `ReSolve()` early-returns while `last_expressions` is empty so no waiting `~` is shown. `Document::InsertGraphSurface` **prefills** the variable rows with `x`/`y` `CodeString`s so the template fields label themselves ("y" above the y bounds in the left column, "x" in the bottom row center); a caret entering a prefilled row stops **before** the string, so passing through a variable row takes three `MoveCaretRight` presses (enter / cross the string / exit). `GraphSurface::ToHtml` (and `GraphLine::ToHtml`) read the marker color bounds-safely — before the first solve `plots` may be shorter than the paragraph list, the fallback color is `default_colors[i % size]`.
- yutovo-calculator: grammar rule `surface_graph` in `src/graph.h` (connected into `graph = line_graph | bar_graph | surface_graph`), AST node `SurfaceGraphNode` in `src/ast.h` — a new graph node needs a `BOOST_FUSION_ADAPT_STRUCT` for **every** number type (8 adaptors, next to the `GraphNode` adaptors) and `Annotation` overloads in `src/annotation.h` (both the main functor and `OperandVisitor`), otherwise parser.cpp fails to compile. The `Solver<Array<Real>>` specialization in `src/solver.cpp` samples the edge-inclusive grid with both loop variables as temp variables; declare the specialization at the bottom of `solver.h`. Tests: `CalcTestArrayReal.surface_graph1..4` in `test/array.cpp`.
- `PlotFormat` gained `SurfaceStyle style` (serialized as `"style"` inside `plot_format`, default 0 — backward compatible); `GetPlotFormat`/`SetPlotFormat` are virtual on `Graph` now (`GetPlotFormat(pos, PlotFormat&)` is used by `CodeParagraph::AfterInsert` for the `█` marker color), and `Document::Get/SetGraphFormat`, `Get/SetPlotFormat`, `GetGraphImage` accept both `GRAPH_LINE` and `GRAPH_SURFACE`. `Paragraph::SetMarker` resets the cached `marker_draw_format` — without the reset the marker keeps drawing with the old color after a plot color change (the cache is otherwise only cleared by `Rescale`).
- Registration points: `editor_utils.cpp` FromJson map (`GRAPH_SURFACE` → `GraphSurface::FromJson`), `UndoGraphSurface` + factory case in `undo.cpp`, `ResultTask::Execute` default branch (save the element id before the first `FindElementOrParent` — the pointer is reassigned to nullptr on the first miss), `ResolveDependenciesTask`'s `resolve_graphs` lambda collects both graph types.
- Tests: `test/graph_surface.cpp` (`FormulaTest.graph_surface1..18`: ToHtml/caret/undo, sampled values (grid step = range/(n-1), edge-inclusive), two surfaces, rotation without re-solve, Shift-pan, wheel zoom row texts, graph/plot formats, error marks, save/load with rotation angles, NaN cells, user-function dependency re-solve, partially filled template does not solve and shows no error, caret traversal right/left through the fields in their visual order, the mouse movement tests also verify `IsChanged` (save -> unchanged -> drag/pan/wheel -> changed), a loaded document becomes changed after a mouse drag, Shift-drag pans through the current projection). To edit the assignment in the dependency test, navigate with `MoveCaretHome`×2 + `MoveCaretUp` + `MoveCaretEnd` + `MoveCaretLeft` from the last graph field (a coordinate click can land on the solved result row instead of the right part). Note the caret granularity: entering a row stops at an element boundary and every typed character is its own `CodeString` (nothing merges, not even with the prefilled "x"/"y"), so the exact number of arrows between two fields depends on the current row contents - collect the counts from `GetEditorState().ToString()` like `graph_surface15` does.
- Front-ends: desktop "Graphs" toolbar action `graph_surface_action` (`MainWindow::GraphSurface`, icon `images/graphs/graph_surface.png`) + context menu "Graph format"/"Copy image" now find `GRAPH_SURFACE` too + `PlotFormatDialog` gained a Style combo (hidden for line graphs via the `surface` ctor flag); web `#graph-surface-button` → `OnGraphSurface`, `PlotFormatDialog` EM_JS/`OnPlotFormat` carry the style, `IsGraph()`/`GetGraphImage()` accept both types (icon copied to `src/site/public/images/graphs/`). Desktop test `TestToolbar::testGraphSurface`; web `test/graph_surface.spec.js` (`utils.insertGraphSurface`).

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

### Comments
Comments that precede a function, method, or class (header/descriptive comments above the definition) start with a capital letter, e.g. `//Group of code paragraphs`, `//TextEquation`, `//Insert a text block into the document text`.

### Lambdas
Place the capture clause on a new line, indented by 4 spaces. Parameters, the `->` return type, and the opening brace follow the normal rules: parameters and return type stay on the same line as the capture clause, and the opening brace goes on its own line.
```cpp
// CORRECT
auto callback =
    [](int value) -> bool
    {
        return value > 0;
    };

auto reference =
    [&]() -> void
    {
        DoWork();
    };

// WRONG
auto callback = [](int value) -> bool {
    return value > 0;
};

auto callback =
    [](int value) -> bool {
    return value > 0;
};
```

## Editor Test Patterns

### Test fixture declarations
Declare new test fixtures (`struct XTest : DocumentTest`, `SolverTest`, ...) in `test/mock.h` next to the other corresponding fixture declarations — do **not** declare them in the individual `test/*.cpp` file (e.g. `TextBlockTest` lives in `mock.h` next to `ArrayTest`/`VariablesTest`, not in `text_block.cpp`). Shared helpers of a fixture are methods of that fixture in `mock.h`.

### Checking the caret state
Editor tests must also verify the caret position - add `ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, ...})) << document.GetEditorState().ToString();` **after** each full `ToHtml()` check (see `TextBlockTest.text_block16`). The braced list is the `ElementId` path of the caret; collect actual values from `GetEditorState().ToString()` (positions before ` [`).

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
In editor tests verify the document by comparing the **full** `ToHtml()` instead of spot checks (`ToText()`, substring searches, element counting) - the complete MathML distinguishes solved results (`ResultRow` wrapping), waiting symbols, formulas vs plain strings, and nested blocks. Check full `ToHtml()` exactly like `SolverSymbolicTest::solver1`:
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

### Undo/Redo in editing tests
Any test that edits an element (typing, inserting subformulas, clearing contents, etc.) must verify both `Undo()` and `Redo()`. Wait for each operation and assert the document state:
```cpp
document.Undo();
document.WaitUndo();
std::this_thread::sleep_for(200ms);
ASSERT_TRUE(document.ToText() == U"...") << ToBasicString(document.ToText());

document.Redo();
document.WaitRedo();
std::this_thread::sleep_for(200ms);
ASSERT_TRUE(document.ToText() == U"...") << ToBasicString(document.ToText());
```

### Async operations and timeouts
Editor and caret methods return a task id, but the editor runs a single thread and executes the commands sequentially - **do not put extra `document.WaitTask()` calls in tests**. Wait only before `document.WaitSolver()` (wrap the `InsertEquation`/`InsertAssignment` call that starts solving) and before checking the result (wrap the last command of a sequence, e.g. `Copy` before using the returned clipboard json, `Save` before reading the file or `Load`ing it, `LoadJson` before checks). Do not rely only on `std::this_thread::sleep_for`.

### Changing config
Do not mutate `document.config` directly and then call `document.SetConfig(document.config, false)` — `SetConfigTask` compares the passed config with the current `document.config`, so a direct mutation makes the comparison see no change. Instead, copy `document.config` into a local `Config`, modify the copy, and pass it to `SetConfig`.

### Emulate user input when building formulas in tests
Editor/solver tests should construct expressions the same way a user would — by calling the document's `Insert*` APIs — not by allocating `Division`/`Power`/`CodeString` objects directly and attaching them with `InsertFormula`. The desktop code path performs caret placement, merging, remake and other logic that direct construction skips, so tests built from raw elements can pass in the test binary but fail on desktop.

```cpp
// WRONG
Division* div = new Division(&document);
// ... manually populate numerator/denominator ...
document.WaitTask(document.InsertFormula(div, true));

// CORRECT
document.WaitTask(document.InsertDivision(true));
document.InsertString("d", true);
document.InsertPower(true);
document.InsertString("2", true);
document.InsertString("g", true);
document.InsertOpenRoundBracket(true);
document.InsertString("x", true);
document.InsertComma(true);
document.InsertString("y", true);
document.InsertCloseRoundBracket(true);
document.WaitTask(document.MoveCaretDown(false));
document.InsertString("d", true);
document.InsertString("x", true);
document.InsertString("d", true);
document.InsertString("y", true);
```

Shared test helpers (e.g. `SolverAutoTest::CreateDerivativeDivision` in `test/mock.h`) must also emulate input rather than construct elements by hand.

### Result-type-specific tests belong in the matching solver file
Tests that explicitly set a non-AUTO result type should be placed in the corresponding test file:
- `ResultType::REAL` → `test/solver_real.cpp` (`SolverRealTest`)
- `ResultType::INTEGER` → `test/solver_integer.cpp` (`SolverIntegerTest`)
- `ResultType::RATIONAL` → `test/solver_rational.cpp` (`SolverRationalTest`)
- `ResultType::COMPLEX` → `test/solver_complex.cpp` (`SolverComplexTest`)
- `ResultType::ARRAY_REAL` → `test/solver_array_real.cpp` (`SolverArrayRealTest`)
- `ResultType::SYMBOLIC_REAL`/`SYMBOLIC_RATIONAL`/`SYMBOLIC_COMPLEX` → `test/solver_derivative.cpp` / `test/solver_symbolic.cpp` / etc., depending on the feature.

AUTO-mode tests and generic derivative tests live in `test/solver_derivative.cpp` (`SolverAutoTest`/`SolverSymbolicTest`).

## File formats
- `.yut` files are gzip-compressed json (not plain text, not a zip archive): `gunzip -c file.yut` shows the json. In release the json is compact, in debug it is pretty printed **and saved uncompressed** (`Document` ctors set `compressed_file = false` under `DEBUG`; `LoadTask` re-enables compression only in release builds).

## Build
Projects are built and tested in `build/debug/`:
```bash
cd build/debug && cmake ../.. && make -j16 yutovo-editor_tests && ./test/yutovo-editor_tests --gtest_filter="FormulaTest.power23"
```

### Run tests from `build/debug`, not from `build/debug/test`
Tests that load documents use paths relative to the current working directory, e.g. `../../test/tests/solver42.yut`. Run the binary as `./test/yutovo-editor_tests ...` with the working directory `build/debug` — from `build/debug/test` those paths resolve to a nonexistent `build/test/tests/` and the test fails with `File not open` and then hangs forever in `WaitLoad`/`WaitSolver` (observed with `solver42` and `lists1`); it looks like a product hang but is a runner error.

## Testing

### Full test suite
Before running the full `yutovo-editor_tests` suite, ask the user at the beginning of the task (after planning) whether to run the complete suite or only targeted tests.

### Debug giac linkage
`yutovo-editor` creates its own imported `giac_imported` target in `src/CMakeLists.txt`. Debug builds must use `${INSTALL_PATH}/lib/libgiacd.a` and release builds `${INSTALL_PATH}/lib/libgiac.a`; linking a debug `yutovo-calculator`/`yutovo-solver` against the release giac library causes an ABI mismatch and memory corruption inside giac (e.g., `CodeTest.code35` crashing in `giac::expand`).

## Network Errors
If an operation fails with a "Network connection failed" error:
1. Wait **2 seconds** and retry automatically on your own.
2. If the retry still fails, continue retrying with a **10-second** interval.

## Repowise
Use `repowise` tools (e.g. `mcp__repowise__get_overview`, `mcp__repowise__get_context`, `mcp__repowise__get_risk`, `mcp__repowise__get_answer`) for codebase exploration, risk analysis, and architecture understanding.
