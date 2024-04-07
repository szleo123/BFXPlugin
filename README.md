# BFXPlugin

Authors: Yilin Li and Linda Zhu

## Configuration
1. Since this Maya Plugin project contains the backend as a submodule, when cloning the repo you need to run:
   ```
   git clone --recursive https://github.com/szleo123/BFXPlugin.git
   ```
3. From the `BFXPlugin` directory, run:
   * Windows: `cmake -B build -S . -G "Visual Studio 17 2022" -A x64` to generate the Visual Studio project.
4. Open the generated `BFX.sln` project inside `/build` directory.
5. Set `BFXPlugin` as the startup project in Visual Studio and build.
   * Refer to the **[Troubleshooting](https://github.com/LinDadaism/BrittleFractureSimulation?tab=readme-ov-file#troubleshooting)** section for the `voro++` build errors.
7. After the project is built, put `fractureUI.mel` in the same directory as the built `.mll` plugin.
   * By default, the `.mll` file is under `/build/bin/[Debug or Release]`.
9. Now you should be able to load the plugin into Maya.

## Dev Notes
[Git - Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
### Updating a repository that contains submodules
If you already have cloned a repository and now want to load it’s submodules you have to use:
```
git submodule update --init
```
### Pulling with submodules
Once you have set up the submodules you can update the repository with `git fetch/pull` like you would normally do. To pull everything including the submodules, use the `--recurse-submodules` or the `--remote` parameter.
```
# pull all changes in the repo including changes in the submodules
git pull --recurse-submodules

# pull all changes for the submodules
# NOTE: Only check out the commit specified in the index of the superproject
git submodule update --remote
```
### Update submodules to the latest commit available from their remote
```
# Change to the submodule directory
cd BFX

# Checkout desired branch
git checkout main

# Update
git pull

# Get back to the superproject root
cd ..

# Now the submodules are in the state you want, so
git commit -am "Pulled down update to BFX"
```
