# Refactor `game.cpp` for Readability and Maintainability

**Goal:** Slim down `game.cpp` by extracting large, logically grouped code blocks into separate modules/files. This will improve readability, maintainability, and make the codebase easier to work with for both humans and AI tools.

---

## 📝 Open Subtasks

- [x] Extract initialization and shutdown logic into a dedicated `GameInitializer` (or similar) module.
- [x] Move input processing (keyboard, mouse, window events) into a new `InputManager` or `GameInput` module. (User confirmed complete)
- [x] Extract world/segment/chunk generation logic into a `WorldSetup` or `WorldInitializer` module.
- [ ] Move mesh rendering and debug rendering logic into a `GameRenderer` or `RenderManager` module.
- [ ] Refactor the main game loop in `game.cpp` to delegate to the new modules, keeping it concise.
- [ ] Update header files and includes to support the new structure.
- [ ] Update any other files that depend on the extracted logic to use the new modules.
- [ ] Test thoroughly after each extraction to ensure no regressions.
- [ ] Document the new structure and update any relevant design docs.

---

**Note:** Only extract code blocks that are long and self-contained. Avoid extracting very short (1-3 line) functions unless they are repeated or clearly belong in a utility module.

**Dependencies:** This refactor may require changes to header files, build scripts, and other modules that interact with `Game`.

---

*This file is not yet linked in `CURRENT_TODO.md` and is for internal tracking of the refactor process.*
>
> **Note:** Testing and executing the refactored code requires explicit user confirmation; tasks remain pending until user verifies build and runtime behavior.
