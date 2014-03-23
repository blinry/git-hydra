git-hydra
=========

A tool that visualizes the internal structure of your Git repository in real-time. With snakes!

It is named after the Hydra from greek mythology, as Git repositories tend to grow a lot of heads, too:

![Screenshot of git-hydra](https://github.com/blinry/git-hydra/raw/master/img/hydra.png)

Dependencies
------------

- libgit2 (https://libgit2.github.com/) >= 0.20
- SFML (http://sfml-dev.org/) in a recent, 2.1-ish version

It's highly recommended to have Git installed, too! :-)

Installation
------------

Type `make`. Add this directory to your PATH.

Usage
-----

When you're inside a git repository, run "git-hydra".

- Move nodes by dragging with the right mouse button.
- Unfold nodes by left clicking them.
- Press "i" to show or hide the index.
- Press "o" to show all objects, not just the reachable ones.
- Press "r" to set all refs as roots, not just HEAD.

You can scroll the commit history and the index with your mouse wheel.

And now, issue some git commands and see what they do!

![How the objects look](https://github.com/blinry/git-hydra/raw/master/img/objects.png)
