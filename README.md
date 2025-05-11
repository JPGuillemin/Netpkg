# Netpkg
The Zenwalk package manager


Netpkg is a tool for easily install or upgrade packages via the network. With Netpkg,
you can make a minimal installation of Zenwalk Linux and install/upgrade just the
packages you need the most.

Basic usage : 
 netpkg pattern1 [pattern2 ...]
 netpkg install pattern1 [pattern2 ...]
 netpkg search [pattern1 pattern2 ...]
 netpkg remove pattern1 [pattern2 ...]
 netpkg help|-h

Output symbols and colors : 
    [I] means "Installed" : remote version is the same as installed package
    [U] means "Updated" : remote version is higher than installed package
    [D] means "Downgraded" : remote version is lower than installed package
    [R] means "Remote" : remote package is not installed yet

Search and install options : 
 netpkg pattern1 [pattern2 ...] : Match package names pattern(s) and PROMPT for action (actions can be : install, remove, reinstall, download)
 netpkg install|-i pattern1 [pattern2 ...] : Same as above

System upgrade options : 
 netpkg update|-u  : reload remotes packages lists
 netpkg upgrade|-uu  : Automatic install of all "updatable" packages

Listing options : 
 netpkg remote|-r  : list REMOTE (not installed) packages
 netpkg changes|-c  : list all CHANGED packages (updated or downgraded)
 netpkg news|-n  : list all NEW (recently added to remote) packages
 netpkg local|-l  : list all LOCAL (installed) packages

Setup commands :
 netpkg remotes|-lr  : list all remotes URL in netpkg.d
 netpkg remote-add|-ra "URL" : add Remote URL to netpkg.d
 netpkg remote-disable|-rd  : disable Remote URL in netpkg.d
 netpkg remote-enable|-re  : enable Remote URL in netpkg.d
 netpkg remote-disable|-rd  : disable dependency processing in netpkg.conf
 netpkg enable-deps|-ed  : enable dependency processing in netpkg.conf
 netpkg mrclean|-mc  : clean the local packages cache
 netpkg dotnew|-z  : search for .new config files in the suystem and prompt for action
