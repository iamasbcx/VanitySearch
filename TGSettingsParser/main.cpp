/*
 * TGSettingsParser - Telegram Desktop Settings File Parser
 * Command Line Interface
 * 
 * Based on Telegram Desktop (https://github.com/telegramdesktop/tdesktop)
 * 
 * Usage:
 *   TGSettingsParser.exe <command> [options]
 * 
 * Commands:
 *   read <tdata_path>              - Read and display all settings
 *   proxy list <tdata_path>        - List all proxy configurations
 *   proxy add <tdata_path> <type> <host> <port> [user] [pass]
 *                                  - Add a new proxy
 *   proxy remove <tdata_path> <index>
 *                                  - Remove proxy by index
 *   proxy set <tdata_path> <index> - Set active proxy
 *   proxy enable <tdata_path>      - Enable proxy
 *   proxy disable <tdata_path>     - Disable proxy (use system)
 *   archive collapsed <tdata_path> <0|1>
 *                                  - Set archive collapsed state
 *   archive menu <tdata_path> <0|1>
 *                                  - Set archive in main menu
 * 
 * Compatible with VS2010
 */

#include "TGSettingsParser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

void printUsage(const char* progName) {
    printf("Telegram Desktop Settings Parser\n");
    printf("Based on Telegram Desktop source code\n");
    printf("\n");
    printf("Usage: %s <command> [options]\n", progName);
    printf("\n");
    printf("Commands:\n");
    printf("  read <tdata_path>              - Read and display all settings\n");
    printf("  proxy list <tdata_path>        - List all proxy configurations\n");
    printf("  proxy add <tdata_path> <type> <host> <port> [user] [pass]\n");
    printf("                                 - Add a new proxy\n");
    printf("                                 - type: socks5, http, mtproto\n");
    printf("  proxy remove <tdata_path> <index>\n");
    printf("                                 - Remove proxy by index\n");
    printf("  proxy select <tdata_path> <index>\n");
    printf("                                 - Select active proxy\n");
    printf("  proxy enable <tdata_path>      - Enable proxy\n");
    printf("  proxy disable <tdata_path>     - Disable proxy (use system)\n");
    printf("  archive collapsed <tdata_path> <0|1>\n");
    printf("                                 - Set archive collapsed state\n");
    printf("  archive menu <tdata_path> <0|1>\n");
    printf("                                 - Set archive in main menu\n");
    printf("\n");
    printf("Notes:\n");
    printf("  - <tdata_path> is the path to Telegram's tdata folder\n");
    printf("  - Default location: %%APPDATA%%\\Telegram Desktop\\tdata\n");
    printf("  - Settings file encryption uses salt-based key (no password needed)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s read \"C:\\Users\\User\\AppData\\Roaming\\Telegram Desktop\\tdata\"\n", progName);
    printf("  %s proxy add \"C:\\tdata\" socks5 127.0.0.1 1080\n", progName);
    printf("  %s proxy add \"C:\\tdata\" http proxy.example.com 8080 user password\n", progName);
}

int cmdRead(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Error: Missing tdata path\n");
        return 1;
    }
    
    const char* tdataPath = argv[2];
    printf("Reading settings from: %s\n\n", tdataPath);
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    printf("\n");
    parser.printSettings();
    parser.printProxies();
    parser.printArchiveSettings();
    
    return 0;
}

int cmdProxyList(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Error: Missing tdata path\n");
        return 1;
    }
    
    const char* tdataPath = argv[3];
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    parser.printProxies();
    return 0;
}

int cmdProxyAdd(int argc, char* argv[]) {
    if (argc < 7) {
        printf("Error: Missing arguments\n");
        printf("Usage: proxy add <tdata_path> <type> <host> <port> [user] [pass]\n");
        return 1;
    }
    
    const char* tdataPath = argv[3];
    const char* typeStr = argv[4];
    const char* host = argv[5];
    int port = atoi(argv[6]);
    const char* user = (argc > 7) ? argv[7] : "";
    const char* pass = (argc > 8) ? argv[8] : "";
    
    ProxyType type = ProxyType_None;
    if (strcmp(typeStr, "socks5") == 0 || strcmp(typeStr, "SOCKS5") == 0) {
        type = ProxyType_Socks5;
    } else if (strcmp(typeStr, "http") == 0 || strcmp(typeStr, "HTTP") == 0) {
        type = ProxyType_Http;
    } else if (strcmp(typeStr, "mtproto") == 0 || strcmp(typeStr, "MTPROTO") == 0) {
        type = ProxyType_Mtproto;
    } else {
        printf("Error: Unknown proxy type '%s'\n", typeStr);
        printf("Valid types: socks5, http, mtproto\n");
        return 1;
    }
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    ProxyData proxy;
    proxy.type = type;
    proxy.host = host;
    proxy.port = port;
    proxy.user = user;
    proxy.password = pass;
    
    if (!parser.addProxy(proxy)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.saveSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    printf("Proxy added successfully\n");
    parser.printProxies();
    return 0;
}

int cmdProxyRemove(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Error: Missing arguments\n");
        printf("Usage: proxy remove <tdata_path> <index>\n");
        return 1;
    }
    
    const char* tdataPath = argv[3];
    int index = atoi(argv[4]);
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.removeProxy(index)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.saveSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    printf("Proxy removed successfully\n");
    parser.printProxies();
    return 0;
}

int cmdProxySelect(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Error: Missing arguments\n");
        printf("Usage: proxy select <tdata_path> <index>\n");
        return 1;
    }
    
    const char* tdataPath = argv[3];
    int index = atoi(argv[4]);
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.selectProxy(index)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.setProxySettings(ProxySettings_Enabled)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.saveSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    printf("Proxy selected and enabled successfully\n");
    parser.printProxies();
    return 0;
}

int cmdProxyEnable(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Error: Missing tdata path\n");
        return 1;
    }
    
    const char* tdataPath = argv[3];
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.setProxySettings(ProxySettings_Enabled)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.saveSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    printf("Proxy enabled successfully\n");
    parser.printProxies();
    return 0;
}

int cmdProxyDisable(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Error: Missing tdata path\n");
        return 1;
    }
    
    const char* tdataPath = argv[3];
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.setProxySettings(ProxySettings_System)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.saveSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    printf("Proxy disabled (using system settings)\n");
    return 0;
}

int cmdArchiveCollapsed(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Error: Missing arguments\n");
        printf("Usage: archive collapsed <tdata_path> <0|1>\n");
        return 1;
    }
    
    const char* tdataPath = argv[3];
    bool collapsed = (atoi(argv[4]) != 0);
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.setArchiveCollapsed(collapsed)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.saveSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    printf("Archive collapsed set to: %s\n", collapsed ? "Yes" : "No");
    return 0;
}

int cmdArchiveMenu(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Error: Missing arguments\n");
        printf("Usage: archive menu <tdata_path> <0|1>\n");
        return 1;
    }
    
    const char* tdataPath = argv[3];
    bool inMainMenu = (atoi(argv[4]) != 0);
    
    TGSettingsParser parser;
    if (!parser.loadSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.setArchiveInMainMenu(inMainMenu)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    if (!parser.saveSettings(tdataPath)) {
        printf("Error: %s\n", parser.getError().c_str());
        return 1;
    }
    
    printf("Archive in main menu set to: %s\n", inMainMenu ? "Yes" : "No");
    return 0;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Enable UTF-8 console output on Windows
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    const char* cmd = argv[1];
    
    if (strcmp(cmd, "read") == 0) {
        return cmdRead(argc, argv);
    }
    else if (strcmp(cmd, "proxy") == 0) {
        if (argc < 3) {
            printf("Error: Missing proxy subcommand\n");
            printUsage(argv[0]);
            return 1;
        }
        
        const char* subcmd = argv[2];
        if (strcmp(subcmd, "list") == 0) {
            return cmdProxyList(argc, argv);
        }
        else if (strcmp(subcmd, "add") == 0) {
            return cmdProxyAdd(argc, argv);
        }
        else if (strcmp(subcmd, "remove") == 0) {
            return cmdProxyRemove(argc, argv);
        }
        else if (strcmp(subcmd, "select") == 0) {
            return cmdProxySelect(argc, argv);
        }
        else if (strcmp(subcmd, "enable") == 0) {
            return cmdProxyEnable(argc, argv);
        }
        else if (strcmp(subcmd, "disable") == 0) {
            return cmdProxyDisable(argc, argv);
        }
        else {
            printf("Error: Unknown proxy subcommand '%s'\n", subcmd);
            printUsage(argv[0]);
            return 1;
        }
    }
    else if (strcmp(cmd, "archive") == 0) {
        if (argc < 3) {
            printf("Error: Missing archive subcommand\n");
            printUsage(argv[0]);
            return 1;
        }
        
        const char* subcmd = argv[2];
        if (strcmp(subcmd, "collapsed") == 0) {
            return cmdArchiveCollapsed(argc, argv);
        }
        else if (strcmp(subcmd, "menu") == 0) {
            return cmdArchiveMenu(argc, argv);
        }
        else {
            printf("Error: Unknown archive subcommand '%s'\n", subcmd);
            printUsage(argv[0]);
            return 1;
        }
    }
    else if (strcmp(cmd, "--help") == 0 || strcmp(cmd, "-h") == 0 || strcmp(cmd, "/?") == 0) {
        printUsage(argv[0]);
        return 0;
    }
    else {
        printf("Error: Unknown command '%s'\n", cmd);
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}
