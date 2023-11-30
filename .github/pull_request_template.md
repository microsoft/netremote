
### Type

- [ ] Bug fix
- [ ] Feature addition
- [X] Feature update
- [ ] Documentation
- [ ] Build Infrastructure

### Side Effects

- [ ] Breaking change
- [ ] Non-functional change

### Goals

Describe what the PR intends to achieve. If the change is a new feature, describe what it is. If the change is a bug fix, reference the issue being fixed. Provide any additional context and motivation for making the change. Eg.
> Simplify maintenance of the event loop. A well-known open source library can be used in place of the custom written code, making it easier to use, understand, and debug.

### Technical Details

Describe how the goals of the PR were achieved. Eg.
> Replace custom event loop code with libevent.

### Test Results

Describe which tests were carried out and a summary of their results. Eg.
> Verified Near Object session completed successfully with the following configurations:
>
> - [X] host as controlee
> - [X] host as controller

### Reviewer Focus

Describe what reviewers should focus on. Eg.

> The event loop could cause the service to exit early if it does not manage object lifetime correctly. Please pay careful attention to how this is achieved in the new event loop.

### Future Work

Describe any future work that is required as a result of this change. Eg.

>
> - Long-running stress testing needs to be completed.
> - The old event loop code needs to be removed once stress-testing with libevent has been completed.
>

### Checklist

- [ ] Build target `all` compiles cleanly.
- [ ] clang-format and clang-tidy deltas produced no new output.
- [ ] Newly added functions include doxygen-style comment block.
