# Embedded Final Project

This repository contains two separate PlatformIO projects for Arduino Uno:

- `arduino1_lidar/`: lidar project; application code in `src/eecs_388_final1.cpp`.
- `arduino2_steering_brake/`: steering and brake project; application code in `src/eecs_388_final2.cpp`.

Each project includes its own `platformio.ini`, source files, headers, and reference documentation. PlatformIO generates its `.pio/` folder locally; Git ignores it.

## First-time setup in VS Code

1. Make sure Git is installed and available to VS Code. No terminal commands are needed for the normal workflow below.
2. Open the Command Palette (Ctrl+Shift+P on Windows/Linux, Cmd+Shift+P on Mac), select **Git: Clone**, and paste:
   `https://github.com/treycurtisKU/Embedded_Final.git`
3. Choose a parent folder in your own writable storage, then open the cloned repository. Keep any old downloaded copies separate until you have preserved work that has not been uploaded.
4. Sign in to GitHub with your own account when prompted. Accept the repository collaborator invitation if you received one; pushing requires write access.
5. Install the **PlatformIO IDE** extension if it is not already installed. Use **File > Open Folder** to open the Arduino project subfolder containing the `platformio.ini` you want to build. VS Code's Source Control can use the Git repository in its parent folder; select **Open Repository** if prompted.

## Every work session

1. Open **View > Source Control**. Before editing, use its **... > Pull** action to get the group's latest changes.
2. Edit the files inside this cloned repository, then save them.
3. Review changes in Source Control. Use **+** to stage the files you intend to share.
4. Enter a short description of your changes and click **Commit**. This saves the changes locally.
5. Choose **... > Push** to upload your commits to GitHub. Complete GitHub sign-in if prompted.

Coordinate before editing the same files. If Push reports that other people have pushed changes, pull their changes, resolve any conflicts, and push again. If VS Code shows a conflict you do not understand, ask a teammate before choosing which version to keep.

Use the clone for ongoing work instead of downloading and uploading whole files through the website. Everyone should use their own GitHub account. On shared lab computers, sign out of GitHub in VS Code and the browser when finished, and avoid saving credentials in a shared operating-system account.
