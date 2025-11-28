/*
 * TGSettingsParser - Telegram Desktop Settings File Parser
 * Based on Telegram Desktop (https://github.com/telegramdesktop/tdesktop)
 * 
 * This tool can:
 * - Read and decrypt Telegram settings file
 * - Parse and display settings data
 * - Modify proxy configuration
 * - Modify archive settings
 * - Re-encrypt and save modified settings
 *
 * Compatible with VS2010
 */

#ifndef TG_SETTINGS_PARSER_H
#define TG_SETTINGS_PARSER_H

#include <string>
#include <vector>
#include <stdint.h>

// File magic and constants
#define TDF_MAGIC "TDF$"
#define TDF_MAGIC_LEN 4
#define LOCAL_ENCRYPT_SALT_SIZE 32
#define LOCAL_ENCRYPT_ITER_COUNT 4000
#define LOCAL_ENCRYPT_NO_PWD_ITER_COUNT 4

// Settings block IDs (from Telegram Desktop)
enum SettingsBlockId {
    dbiDcOptionOldOld = 0x00,
    dbiDcOptionOld = 0x01,
    dbiDcOptions = 0x02,
    dbiChatSizeMax = 0x03,
    dbiMutePeer = 0x04,
    dbiSendKey = 0x05,
    dbiAutoStart = 0x06,
    dbiStartMinimized = 0x07,
    dbiSoundFlashBounceNotify = 0x08,
    dbiWorkMode = 0x09,
    dbiSeenTrayTooltip = 0x0a,
    dbiDesktopNotify = 0x0b,
    dbiAutoUpdate = 0x0c,
    dbiLastUpdateCheck = 0x0d,
    dbiWindowPosition = 0x0e,
    dbiConnectionTypeOld = 0x0f,
    dbiDefaultAttach = 0x11,
    dbiCatsAndDogs = 0x12,
    dbiReplaceEmoji = 0x13,
    dbiAskDownloadPath = 0x14,
    dbiDownloadPathOld = 0x15,
    dbiScalePercent = 0x16,
    dbiEmojiTab = 0x17,
    dbiRecentEmojiOldOld = 0x18,
    dbiLoggedPhoneNumber = 0x19,
    dbiMutedPeers = 0x1a,
    dbiNotifyView = 0x1c,
    dbiSendToMenu = 0x1d,
    dbiCompressPastedImage = 0x1e,
    dbiLangOld = 0x1f,
    dbiLangFileOld = 0x20,
    dbiTileBackground = 0x21,
    dbiAutoLock = 0x22,
    dbiDialogLastPath = 0x23,
    dbiRecentEmojiOld = 0x24,
    dbiEmojiVariantsOld = 0x25,
    dbiRecentStickers = 0x26,
    dbiDcOption = 0x27,
    dbiTryIPv6 = 0x28,
    dbiSongVolume = 0x29,
    dbiWindowsNotificationsOld = 0x30,
    dbiIncludeMuted = 0x31,
    dbiMegagroupSizeMax = 0x32,
    dbiDownloadPath = 0x33,
    dbiAutoDownloadOld = 0x34,
    dbiSavedGifsLimit = 0x35,
    dbiShowingSavedGifsOld = 0x36,
    dbiAutoPlayOld = 0x37,
    dbiAdaptiveForWide = 0x38,
    dbiHiddenPinnedMessages = 0x39,
    dbiRecentEmoji = 0x3a,
    dbiEmojiVariants = 0x3b,
    dbiDialogsMode = 0x40,
    dbiModerateMode = 0x41,
    dbiVideoVolume = 0x42,
    dbiStickersRecentLimit = 0x43,
    dbiNativeNotifications = 0x44,
    dbiNotificationsCount = 0x45,
    dbiNotificationsCorner = 0x46,
    dbiThemeKeyOld = 0x47,
    dbiDialogsWidthRatioOld = 0x48,
    dbiUseExternalVideoPlayer = 0x49,
    dbiDcOptions2 = 0x4a,
    dbiLangPackKey = 0x4b,
    dbiConnectionType = 0x4c,
    dbiStickersFavedLimit = 0x4d,
    dbiSuggestStickersByEmoji = 0x4e,
    dbiSuggestEmoji = 0x4f,
    dbiTxtDomainString = 0x50,
    dbiThemeKey = 0x51,
    dbiTileDay = 0x52,
    dbiTileNight = 0x53,
    dbiDialogsWidthRatio = 0x54,
    dbiLanguagesKey = 0x55,
    dbiPlaybackSpeed = 0x56,
    dbiVoicePlaybackSpeed = 0x57,
    dbiFallbackProductionConfig = 0x60,
    dbiBackgroundKey = 0x61,
    dbiEncryptedWithSalt = 0x333,
    dbiEncrypted = 0x444,
    dbiVersion = 0x666,
    dbiApplicationSettings = 0x99,
    dbiPowerSaving = 0x9a,
};

// Proxy types
enum ProxyType {
    ProxyType_None = 0,
    ProxyType_Socks5 = 1,
    ProxyType_Http = 2,
    ProxyType_Mtproto = 3
};

// Proxy settings
enum ProxySettings {
    ProxySettings_System = 0,
    ProxySettings_Enabled = 1,
    ProxySettings_Disabled = 2
};

// Proxy configuration structure
struct ProxyData {
    ProxyType type;
    std::string host;
    uint32_t port;
    std::string user;
    std::string password;
    
    ProxyData() : type(ProxyType_None), port(0) {}
};

// Archive settings structure
struct ArchiveSettings {
    bool archiveCollapsed;
    bool archiveInMainMenu;
    int32_t autoArchiveSettingAvailable;
    
    ArchiveSettings() : archiveCollapsed(false), archiveInMainMenu(false), autoArchiveSettingAvailable(0) {}
};

// Settings data container
struct TGSettings {
    // Basic settings
    int32_t autoStart;
    int32_t startMinimized;
    int32_t sendToMenu;
    int32_t seenTrayTooltip;
    int32_t autoUpdate;
    int32_t lastUpdateCheck;
    int32_t scalePercent;
    
    // Proxy settings
    ProxySettings proxySettings;
    std::vector<ProxyData> proxies;
    int32_t selectedProxy;
    
    // Archive settings
    ArchiveSettings archive;
    
    // Theme keys
    uint64_t themeKeyDay;
    uint64_t themeKeyNight;
    int32_t nightMode;
    
    // Language
    uint64_t langPackKey;
    uint64_t languagesKey;
    
    // Other
    std::string dialogLastPath;
    int32_t powerSaving;
    
    TGSettings() : 
        autoStart(0), startMinimized(0), sendToMenu(0), seenTrayTooltip(0),
        autoUpdate(1), lastUpdateCheck(0), scalePercent(100),
        proxySettings(ProxySettings_System), selectedProxy(-1),
        themeKeyDay(0), themeKeyNight(0), nightMode(0),
        langPackKey(0), languagesKey(0), powerSaving(0) {}
};

// Main parser class
class TGSettingsParser {
public:
    TGSettingsParser();
    ~TGSettingsParser();
    
    // Load settings from Telegram data directory
    // Note: Settings file uses salt-based encryption (empty password)
    // The passcode parameter is reserved for future account data decryption (key_datas)
    bool loadSettings(const std::string& tdataPath, const std::string& passcode = "");
    
    // Get parsed settings
    const TGSettings& getSettings() const { return m_settings; }
    
    // Modify proxy configuration
    bool setProxy(const ProxyData& proxy);
    bool addProxy(const ProxyData& proxy);
    bool removeProxy(int index);
    bool setProxySettings(ProxySettings settings);
    bool selectProxy(int index);
    
    // Modify archive settings
    bool setArchiveCollapsed(bool collapsed);
    bool setArchiveInMainMenu(bool inMainMenu);
    
    // Save modified settings
    bool saveSettings(const std::string& tdataPath);
    
    // Display settings
    void printSettings() const;
    void printProxies() const;
    void printArchiveSettings() const;
    
    // Get error message
    const std::string& getError() const { return m_error; }
    
private:
    // Encryption/Decryption
    bool createLocalKey(const std::string& passcode, const uint8_t* salt, size_t saltLen);
    bool decryptData(const uint8_t* encrypted, size_t encryptedLen, std::vector<uint8_t>& decrypted);
    bool encryptData(const std::vector<uint8_t>& data, std::vector<uint8_t>& encrypted);
    
    // Parsing
    bool parseSettingsFile(const std::string& filePath);
    bool parseSettingsBlock(uint32_t blockId, const uint8_t* data, size_t dataLen, int32_t version);
    
    // File I/O
    bool readFile(const std::string& filePath, std::vector<uint8_t>& data);
    bool writeFile(const std::string& filePath, const std::vector<uint8_t>& data);
    
    // Serialization
    void serializeSettings(std::vector<uint8_t>& data);
    
    // Members
    TGSettings m_settings;
    std::vector<uint8_t> m_localKey; // 256-bit key
    std::vector<uint8_t> m_salt;
    std::string m_error;
    bool m_keyCreated;
    int32_t m_version;
};

#endif // TG_SETTINGS_PARSER_H
