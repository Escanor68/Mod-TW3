# Witcher Online server (source)

Java UDP server for Witcher Online. Default listen port: **40000**.

## Requirements

- **JDK 21+** to compile (upstream packaging targets modern Java).
- Runtime: same JRE/JDK; place config files beside the JAR (`appDir()` = JAR directory).

## Build and run

```bash
cd server
mkdir -p out
javac --release 21 -d out src/*.java
jar cfe out/WitcherServer.jar WitcherServer -C out .
cp server.properties.example out/server.properties   # optional
java -jar out/WitcherServer.jar
# or: java -jar out/WitcherServer.jar 40000
```

Console: type `help` or `?` for kick/ban/whitelist/stats commands.  
Operator identity notes: [docs/hosting/session-identity.md](../docs/hosting/session-identity.md).

## Tests (session identity rules)

```bash
./scripts/run-identity-tests.sh
# optional: export JAVA_HOME=/path/to/jdk-21
```

## Hosting outside your LAN

See [docs/hosting/dedicated-hosting.md](../docs/hosting/dedicated-hosting.md).

Released JAR builds are also distributed on the Nexus **Witcher Online Server** package (see upstream wiki).
