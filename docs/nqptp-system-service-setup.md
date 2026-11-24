# NQPTP system service setup on Debian Trixie

This guide describes a local source installation on Debian 13 (Trixie) with
systemd. A Debian package is preferable when the installation needs
package-managed upgrades and removal, or deployment to multiple machines.

The proposed filesystem layout follows the Filesystem Hierarchy Standard:

- `/usr/local/bin/nqptp`: locally installed executable.
- `/etc/systemd/system/nqptp.service`: locally administered service unit.
- The systemd journal: service logs.

NQPTP currently needs no configuration file or persistent state directory.
The source workspace remains separate from the installed executable.

## Build and install

Install the project's build prerequisites first. From the repository root, build
as an ordinary user and install as root:

```sh
autoreconf -fi
./configure --prefix=/usr/local
make
sudo -A make install
/usr/local/bin/nqptp -V
```

Running `autoreconf` and `configure` regenerates the build files for the current
workspace. The configure command deliberately omits `--with-systemd-startup`:
the locally administered unit below is authoritative. Check for an existing
NQPTP installation or unit before installing to avoid conflicting versions.

## Create the service unit

Create `/etc/systemd/system/nqptp.service` with a root-privileged editor:

```ini
[Unit]
Description=NQPTP timing daemon
After=network.target
Before=shairport-sync.service

[Service]
Type=simple
ExecStart=/usr/local/bin/nqptp
DynamicUser=yes
AmbientCapabilities=CAP_NET_BIND_SERVICE
CapabilityBoundingSet=CAP_NET_BIND_SERVICE
LimitRTPRIO=6
Restart=on-failure

NoNewPrivileges=yes
ProtectSystem=strict
ProtectHome=yes
PrivateTmp=yes
ProtectKernelTunables=yes
ProtectKernelModules=yes
ProtectControlGroups=yes
RestrictSUIDSGID=yes
RestrictAddressFamilies=AF_UNIX AF_INET AF_INET6
SystemCallArchitectures=native

[Install]
WantedBy=multi-user.target
```

This retains the dynamic user, privileged-port capability and real-time priority
limit from the project's [service template](../nqptp.service.in). The hardening
settings are a starting point: validate them with the actual client and
network environment before relying on them.

### Running as a dynamic user

The unit above contains the steps needed to run NQPTP without root:

1. `DynamicUser=yes` tells systemd to allocate a service user and group when
   starting the service and release them when it stops. No `useradd`, password
   or manual ownership setup is needed. If a static user or group with the
   selected name already exists, systemd uses it instead.
2. `AmbientCapabilities=CAP_NET_BIND_SERVICE` lets the unprivileged process
   bind the normally privileged UDP ports 319 and 320.
   `CapabilityBoundingSet=CAP_NET_BIND_SERVICE` limits the capabilities the
   service can acquire. Do not give the executable a setuid bit or file
   capabilities.
3. `LimitRTPRIO=6` allows the daemon to request its real-time scheduling
   priority without running as root. `NoNewPrivileges=yes` prevents the service
   from gaining additional privileges through later program execution.
4. Install the executable and unit with administrator privileges, then let
   systemd start the daemon. Administrator privileges are needed for
   installation and service management, not for the daemon's normal runtime.

The dynamic numeric identity can change between starts and be reused. Do not
rely on it for persistent file ownership. NQPTP currently needs no persistent
directory, and its executable must remain readable and executable by the
service user.

Do not isolate the host network or `/dev/shm`: NQPTP needs the host network and
POSIX shared memory accessible to Shairport Sync. The current implementation
creates shared-memory objects that are writable by other local users; the
systemd settings above do not eliminate that trust boundary.

`After=network.target` provides ordering, not a guarantee that the network is
fully configured. `Before=shairport-sync.service` orders the two services when
both are started, but does not cause Shairport Sync to start NQPTP. If that
dependency is required, add a Shairport Sync drop-in containing:

```ini
[Unit]
Wants=nqptp.service
After=nqptp.service
```

## Activate and verify

After creating the unit:

```sh
sudo -A systemd-analyze verify /etc/systemd/system/nqptp.service
sudo -A systemctl daemon-reload
sudo -A systemctl enable --now nqptp
sudo -A systemctl restart shairport-sync

systemctl status nqptp
journalctl -u nqptp -b
systemd-analyze security nqptp.service
```

Once the service is active, confirm its process does not run as UID 0:

```sh
ps -o user,uid,group,gid,args -p "$(systemctl show --value --property=MainPID nqptp)"
```

Inspect the configured identity and capability settings with:

```sh
systemctl show nqptp -p DynamicUser -p User -p AmbientCapabilities -p CapabilityBoundingSet
```

Restart Shairport Sync only if it is installed and running. Check the journal
for errors, confirm UDP ports 319 and 320 are available to NQPTP, and test actual
AirPlay 2 playback. Firewall rules must permit the required PTP traffic. The
control port, UDP 9000, is bound to localhost.

Shared-memory objects are created when clients request them, so their absence
immediately after startup is not necessarily a failure. `systemd-analyze
security` is useful for reviewing hardening but is not a substitute for
functional testing or a security audit.