# Netpkg

Netpkg is a lightweight package manager for **Zenwalk Linux** and **Slackware-based systems**.
It allows users to **install, upgrade, remove, and search packages from remote repositories** while keeping the system simple and minimal.

The tool is written entirely in **Bash** and focuses on transparency, simplicity, and minimal dependencies.

Originally developed by **Jean-Philippe Guillemin**, this version includes simplified internal implementations of package installation, upgrading, and removal.

---

# Features

- Install packages from remote repositories
- Upgrade installed packages
- Remove installed packages
- Search packages locally and remotely
- Optional dependency handling
- Package cache support
- Repository management
- MD5 checksum verification
- `.new` configuration file management
- Kernel installation helper
- Minimal system requirements

---

# How It Works

Netpkg synchronizes repository metadata and builds simple local databases to track packages.

Databases used:

| File | Purpose |
|-----|--------|
| `RemotePkgs.db` | Available packages from remotes |
| `RemoteDeps.db` | Dependency information |
| `LocalPkgs.db` | Installed packages |
| `PACKAGES.TXT` | Repository metadata |

---

# Installation

Clone the repository:

```bash
git clone https://github.com/yourusername/netpkg.git
cd netpkg
```

Make the script executable:

```bash
chmod +x netpkg
```

Copy it into your system path:

```bash
sudo cp netpkg /usr/sbin/netpkg
```

---

# Basic Usage

### Update repository metadata

```bash
netpkg update
```

### Upgrade system packages

```bash
netpkg upgrade
```

### Install a package

```bash
netpkg install package-name
```

### Search packages

```bash
netpkg search keyword
```

### Remove a package

```bash
netpkg remove package-name
```

---

# Repository Management

List configured repositories:

```bash
netpkg remotes
```

Add a new repository:

```bash
netpkg remote-add http://repository-url
```

Disable a repository:

```bash
netpkg remote-disable
```

Enable a repository:

```bash
netpkg remote-enable
```

---

# Dependency Handling

Dependency processing can be toggled:

Enable:

```bash
netpkg enable-deps
```

Disable:

```bash
netpkg disable-deps
```

Dependencies are parsed from repository metadata when available.

---

# Configuration

Configuration file:

```
/etc/netpkg.conf
```

Important settings:

| Option | Description |
|------|-------------|
| `Netpkg_dir` | Working directory |
| `Remotes_dir` | Repository configuration directory |
| `Local_repository` | Local package cache |
| `Handle_dependencies` | Enable dependency processing |
| `Keep_packages` | Keep downloaded packages after install |
| `Protected_files` | Files protected during upgrades |

---

# Handling `.new` Configuration Files

After upgrades, configuration files ending with `.new` may appear.

To manage them:

```bash
netpkg dotnew
```

This allows you to:

- replace the original file
- view differences
- skip
- delete the new file

---

# Cache Cleaning

Remove cached packages:

```bash
netpkg mrclean
```

---

# Status Symbols

| Symbol | Meaning |
|------|--------|
| `[I]` | Installed (same version) |
| `[U]` | Update available |
| `[D]` | Downgrade available |
| `[R]` | Remote package not installed |

---

# License

This project is licensed under the **GNU General Public License v2 or later**.

See:
http://www.gnu.org/copyleft/gpl.html

---

# Author

Original Author
Jean-Philippe Guillemin
h1p8r10n@yandex.com

---

# Contributing

Contributions are welcome.

You can help by:

- improving repository parsing
- enhancing dependency handling
- improving performance
- adding better error handling
- extending package verification

Submit pull requests or open issues.

---

# Disclaimer

Netpkg performs direct filesystem operations similar to traditional Slackware tools.
Use it with appropriate privileges and caution on production systems.
