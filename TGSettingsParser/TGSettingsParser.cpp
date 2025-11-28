/*
 * TGSettingsParser - Implementation
 * Based on Telegram Desktop (https://github.com/telegramdesktop/tdesktop)
 */

#include "TGSettingsParser.h"
#include "TGCrypto.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define PATH_SEPARATOR '\\'
#else
#include <sys/stat.h>
#define PATH_SEPARATOR '/'
#endif

// Helper class for reading binary data
class DataReader {
public:
    DataReader(const uint8_t* data, size_t size) 
        : m_data(data), m_size(size), m_pos(0) {}
    
    bool read(void* dest, size_t len) {
        if (m_pos + len > m_size) return false;
        memcpy(dest, m_data + m_pos, len);
        m_pos += len;
        return true;
    }
    
    bool readUint32(uint32_t& val) {
        return read(&val, sizeof(val));
    }
    
    bool readInt32(int32_t& val) {
        return read(&val, sizeof(val));
    }
    
    bool readUint64(uint64_t& val) {
        return read(&val, sizeof(val));
    }
    
    bool readString(std::string& str) {
        uint32_t len;
        if (!readUint32(len)) return false;
        
        if (len == 0xFFFFFFFF) {
            str.clear();
            return true;
        }
        
        if (m_pos + len > m_size) return false;
        str.assign((const char*)(m_data + m_pos), len);
        m_pos += len;
        return true;
    }
    
    bool readByteArray(std::vector<uint8_t>& arr) {
        uint32_t len;
        if (!readUint32(len)) return false;
        
        if (len == 0xFFFFFFFF) {
            arr.clear();
            return true;
        }
        
        if (m_pos + len > m_size) return false;
        arr.assign(m_data + m_pos, m_data + m_pos + len);
        m_pos += len;
        return true;
    }
    
    size_t remaining() const { return m_size - m_pos; }
    size_t position() const { return m_pos; }
    const uint8_t* current() const { return m_data + m_pos; }
    bool atEnd() const { return m_pos >= m_size; }
    
    void skip(size_t len) { 
        m_pos += len;
        if (m_pos > m_size) m_pos = m_size;
    }
    
private:
    const uint8_t* m_data;
    size_t m_size;
    size_t m_pos;
};

// Helper class for writing binary data
class DataWriter {
public:
    DataWriter() {}
    
    void write(const void* src, size_t len) {
        const uint8_t* bytes = (const uint8_t*)src;
        m_data.insert(m_data.end(), bytes, bytes + len);
    }
    
    void writeUint32(uint32_t val) {
        write(&val, sizeof(val));
    }
    
    void writeInt32(int32_t val) {
        write(&val, sizeof(val));
    }
    
    void writeUint64(uint64_t val) {
        write(&val, sizeof(val));
    }
    
    void writeString(const std::string& str) {
        if (str.empty()) {
            writeUint32(0);
        } else {
            writeUint32((uint32_t)str.size());
            write(str.c_str(), str.size());
        }
    }
    
    void writeByteArray(const std::vector<uint8_t>& arr) {
        if (arr.empty()) {
            writeUint32(0);
        } else {
            writeUint32((uint32_t)arr.size());
            write(&arr[0], arr.size());
        }
    }
    
    const std::vector<uint8_t>& getData() const { return m_data; }
    std::vector<uint8_t>& getData() { return m_data; }
    size_t size() const { return m_data.size(); }
    
private:
    std::vector<uint8_t> m_data;
};

TGSettingsParser::TGSettingsParser() 
    : m_keyCreated(false), m_version(0) {
    m_localKey.resize(32, 0);
}

TGSettingsParser::~TGSettingsParser() {
}

bool TGSettingsParser::createLocalKey(const std::string& passcode, const uint8_t* salt, size_t saltLen) {
    if (saltLen != LOCAL_ENCRYPT_SALT_SIZE) {
        m_error = "Invalid salt size";
        return false;
    }
    
    m_salt.assign(salt, salt + saltLen);
    
    int iterations = passcode.empty() ? LOCAL_ENCRYPT_NO_PWD_ITER_COUNT : LOCAL_ENCRYPT_ITER_COUNT;
    
    // Use PBKDF2-HMAC-SHA1 to derive key (legacy Telegram method)
    if (!TGCrypto::pbkdf2_hmac_sha1(
            (const uint8_t*)passcode.c_str(), passcode.size(),
            salt, saltLen,
            iterations,
            &m_localKey[0], 32)) {
        m_error = "Failed to derive encryption key";
        return false;
    }
    
    m_keyCreated = true;
    return true;
}

bool TGSettingsParser::decryptData(const uint8_t* encrypted, size_t encryptedLen, std::vector<uint8_t>& decrypted) {
    if (!m_keyCreated) {
        m_error = "Encryption key not created";
        return false;
    }
    
    if (encryptedLen <= 16 || (encryptedLen & 0x0F) != 0) {
        m_error = "Invalid encrypted data size";
        return false;
    }
    
    size_t fullLen = encryptedLen - 16;
    decrypted.resize(fullLen);
    
    const uint8_t* encryptedKey = encrypted;
    const uint8_t* encryptedData = encrypted + 16;
    
    // AES-256-IGE decrypt
    if (!TGCrypto::aesDecryptLocal(encryptedData, &decrypted[0], fullLen, 
                                    &m_localKey[0], encryptedKey)) {
        m_error = "AES decryption failed";
        return false;
    }
    
    // Verify SHA1 hash
    uint8_t sha1Buffer[20];
    TGCrypto::sha1(&decrypted[0], decrypted.size(), sha1Buffer);
    
    if (memcmp(sha1Buffer, encryptedKey, 16) != 0) {
        m_error = "Bad decrypt key - incorrect password?";
        return false;
    }
    
    // Get actual data length
    uint32_t dataLen = *(const uint32_t*)&decrypted[0];
    if (dataLen > decrypted.size() || dataLen <= fullLen - 16 || dataLen < 4) {
        m_error = "Bad decrypted data size";
        return false;
    }
    
    // Remove the length prefix and extra padding
    decrypted.erase(decrypted.begin(), decrypted.begin() + 4);
    decrypted.resize(dataLen - 4);
    
    return true;
}

bool TGSettingsParser::encryptData(const std::vector<uint8_t>& data, std::vector<uint8_t>& encrypted) {
    if (!m_keyCreated) {
        m_error = "Encryption key not created";
        return false;
    }
    
    // Prepare data with length prefix
    std::vector<uint8_t> toEncrypt;
    uint32_t dataLen = (uint32_t)(data.size() + 4); // Include length field
    toEncrypt.resize(4);
    memcpy(&toEncrypt[0], &dataLen, 4);
    toEncrypt.insert(toEncrypt.end(), data.begin(), data.end());
    
    // Pad to 16 bytes
    uint32_t fullLen = (uint32_t)toEncrypt.size();
    if (fullLen & 0x0F) {
        size_t padding = 16 - (fullLen & 0x0F);
        toEncrypt.resize(fullLen + padding, 0);
        TGCrypto::randomFill(&toEncrypt[fullLen], padding);
        fullLen = (uint32_t)toEncrypt.size();
    }
    
    // Calculate SHA1 hash for key
    uint8_t sha1Buffer[20];
    TGCrypto::sha1(&toEncrypt[0], toEncrypt.size(), sha1Buffer);
    
    // Allocate output buffer
    encrypted.resize(16 + fullLen);
    memcpy(&encrypted[0], sha1Buffer, 16);
    
    // AES-256-IGE encrypt
    if (!TGCrypto::aesEncryptLocal(&toEncrypt[0], &encrypted[16], fullLen,
                                    &m_localKey[0], sha1Buffer)) {
        m_error = "AES encryption failed";
        return false;
    }
    
    return true;
}

bool TGSettingsParser::readFile(const std::string& filePath, std::vector<uint8_t>& data) {
    FILE* f = fopen(filePath.c_str(), "rb");
    if (!f) {
        m_error = "Cannot open file: " + filePath;
        return false;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size <= 0) {
        fclose(f);
        m_error = "File is empty: " + filePath;
        return false;
    }
    
    data.resize(size);
    size_t read = fread(&data[0], 1, size, f);
    fclose(f);
    
    if (read != (size_t)size) {
        m_error = "Failed to read file: " + filePath;
        return false;
    }
    
    return true;
}

bool TGSettingsParser::writeFile(const std::string& filePath, const std::vector<uint8_t>& data) {
    FILE* f = fopen(filePath.c_str(), "wb");
    if (!f) {
        m_error = "Cannot open file for writing: " + filePath;
        return false;
    }
    
    size_t written = fwrite(&data[0], 1, data.size(), f);
    fclose(f);
    
    if (written != data.size()) {
        m_error = "Failed to write file: " + filePath;
        return false;
    }
    
    return true;
}

bool TGSettingsParser::loadSettings(const std::string& tdataPath, const std::string& passcode) {
    // Build settings file path
    std::string settingsPath = tdataPath;
    if (!settingsPath.empty() && settingsPath[settingsPath.size()-1] != PATH_SEPARATOR) {
        settingsPath += PATH_SEPARATOR;
    }
    settingsPath += "settings";
    
    // Try different file extensions
    std::string filePaths[] = {
        settingsPath + "s",  // Modern format
        settingsPath + "0",  // Legacy format
        settingsPath + "1"   // Legacy backup
    };
    
    std::vector<uint8_t> fileData;
    bool found = false;
    std::string usedPath;
    
    for (int i = 0; i < 3; i++) {
        if (readFile(filePaths[i], fileData)) {
            usedPath = filePaths[i];
            found = true;
            break;
        }
    }
    
    if (!found) {
        m_error = "Settings file not found in: " + tdataPath;
        return false;
    }
    
    printf("[+] Found settings file: %s\n", usedPath.c_str());
    printf("[+] File size: %u bytes\n", (unsigned)fileData.size());
    
    // Check magic
    if (fileData.size() < TDF_MAGIC_LEN) {
        m_error = "File too small";
        return false;
    }
    
    if (memcmp(&fileData[0], TDF_MAGIC, TDF_MAGIC_LEN) != 0) {
        m_error = "Invalid file magic";
        return false;
    }
    
    // Read version
    if (fileData.size() < TDF_MAGIC_LEN + 4) {
        m_error = "Cannot read version";
        return false;
    }
    
    memcpy(&m_version, &fileData[TDF_MAGIC_LEN], 4);
    printf("[+] File version: %d\n", m_version);
    
    // Read data (skip magic and version)
    size_t dataStart = TDF_MAGIC_LEN + 4;
    size_t dataSize = fileData.size() - dataStart - 16; // 16 bytes for MD5 signature
    
    if (dataSize <= 0) {
        m_error = "No data in file";
        return false;
    }
    
    // Verify MD5 signature
    uint8_t md5Result[16];
    const uint8_t* data = &fileData[dataStart];
    int32_t dataSizeInt = (int32_t)dataSize;
    
    TGCrypto::md5Init();
    TGCrypto::md5Update(data, dataSize);
    TGCrypto::md5Update(&dataSizeInt, sizeof(dataSizeInt));
    TGCrypto::md5Update(&m_version, sizeof(m_version));
    TGCrypto::md5Update(TDF_MAGIC, TDF_MAGIC_LEN);
    TGCrypto::md5Final(md5Result);
    
    if (memcmp(md5Result, data + dataSize, 16) != 0) {
        m_error = "File signature verification failed";
        return false;
    }
    
    printf("[+] File signature verified\n");
    
    // Parse the data stream
    DataReader reader(data, dataSize);
    
    // Read salt and encrypted data
    std::vector<uint8_t> salt;
    std::vector<uint8_t> settingsEncrypted;
    
    if (!reader.readByteArray(salt) || !reader.readByteArray(settingsEncrypted)) {
        m_error = "Failed to read salt/encrypted data";
        return false;
    }
    
    printf("[+] Salt size: %u bytes\n", (unsigned)salt.size());
    printf("[+] Encrypted data size: %u bytes\n", (unsigned)settingsEncrypted.size());
    
    if (salt.size() != LOCAL_ENCRYPT_SALT_SIZE) {
        m_error = "Invalid salt size";
        return false;
    }
    
    // Create decryption key
    if (!createLocalKey(passcode, &salt[0], salt.size())) {
        return false;
    }
    
    printf("[+] Decryption key created\n");
    
    // Decrypt settings
    std::vector<uint8_t> decrypted;
    if (!decryptData(&settingsEncrypted[0], settingsEncrypted.size(), decrypted)) {
        return false;
    }
    
    printf("[+] Settings decrypted, size: %u bytes\n", (unsigned)decrypted.size());
    
    // Parse decrypted settings
    DataReader settingsReader(&decrypted[0], decrypted.size());
    
    while (!settingsReader.atEnd()) {
        uint32_t blockId;
        if (!settingsReader.readUint32(blockId)) {
            break;
        }
        
        if (!parseSettingsBlock(blockId, settingsReader.current(), settingsReader.remaining(), m_version)) {
            printf("[!] Warning: Failed to parse block 0x%x\n", blockId);
        }
    }
    
    printf("[+] Settings parsing completed\n");
    return true;
}

bool TGSettingsParser::parseSettingsBlock(uint32_t blockId, const uint8_t* data, size_t dataLen, int32_t version) {
    DataReader reader(data, dataLen);
    
    switch (blockId) {
        case dbiAutoStart: {
            int32_t val;
            if (!reader.readInt32(val)) return false;
            m_settings.autoStart = val;
            printf("  [Settings] AutoStart: %d\n", val);
            break;
        }
        
        case dbiStartMinimized: {
            int32_t val;
            if (!reader.readInt32(val)) return false;
            m_settings.startMinimized = val;
            printf("  [Settings] StartMinimized: %d\n", val);
            break;
        }
        
        case dbiSendToMenu: {
            int32_t val;
            if (!reader.readInt32(val)) return false;
            m_settings.sendToMenu = val;
            printf("  [Settings] SendToMenu: %d\n", val);
            break;
        }
        
        case dbiSeenTrayTooltip: {
            int32_t val;
            if (!reader.readInt32(val)) return false;
            m_settings.seenTrayTooltip = val;
            printf("  [Settings] SeenTrayTooltip: %d\n", val);
            break;
        }
        
        case dbiAutoUpdate: {
            int32_t val;
            if (!reader.readInt32(val)) return false;
            m_settings.autoUpdate = val;
            printf("  [Settings] AutoUpdate: %d\n", val);
            break;
        }
        
        case dbiLastUpdateCheck: {
            int32_t val;
            if (!reader.readInt32(val)) return false;
            m_settings.lastUpdateCheck = val;
            printf("  [Settings] LastUpdateCheck: %d\n", val);
            break;
        }
        
        case dbiScalePercent: {
            int32_t val;
            if (!reader.readInt32(val)) return false;
            m_settings.scalePercent = val;
            printf("  [Settings] ScalePercent: %d\n", val);
            break;
        }
        
        case dbiDialogLastPath: {
            std::string path;
            if (!reader.readString(path)) return false;
            m_settings.dialogLastPath = path;
            printf("  [Settings] DialogLastPath: %s\n", path.c_str());
            break;
        }
        
        case dbiThemeKey: {
            uint64_t keyDay, keyNight;
            uint32_t nightMode;
            if (!reader.readUint64(keyDay)) return false;
            if (!reader.readUint64(keyNight)) return false;
            if (!reader.readUint32(nightMode)) return false;
            m_settings.themeKeyDay = keyDay;
            m_settings.themeKeyNight = keyNight;
            m_settings.nightMode = nightMode;
            printf("  [Settings] ThemeKeyDay: 0x%llx, ThemeKeyNight: 0x%llx, NightMode: %d\n", 
                   (unsigned long long)keyDay, (unsigned long long)keyNight, nightMode);
            break;
        }
        
        case dbiLangPackKey: {
            uint64_t val;
            if (!reader.readUint64(val)) return false;
            m_settings.langPackKey = val;
            printf("  [Settings] LangPackKey: 0x%llx\n", (unsigned long long)val);
            break;
        }
        
        case dbiLanguagesKey: {
            uint64_t val;
            if (!reader.readUint64(val)) return false;
            m_settings.languagesKey = val;
            printf("  [Settings] LanguagesKey: 0x%llx\n", (unsigned long long)val);
            break;
        }
        
        case dbiPowerSaving: {
            int32_t val;
            if (!reader.readInt32(val)) return false;
            m_settings.powerSaving = val;
            printf("  [Settings] PowerSaving: %d\n", val);
            break;
        }
        
        case dbiConnectionType: {
            // Parse proxy settings
            int32_t proxyType;
            if (!reader.readInt32(proxyType)) return false;
            
            m_settings.proxySettings = (ProxySettings)proxyType;
            printf("  [Settings] ProxySettings: %d\n", proxyType);
            
            if (proxyType == ProxySettings_Enabled) {
                // Read proxy count and proxies
                int32_t proxyCount;
                if (!reader.readInt32(proxyCount)) return false;
                
                printf("  [Settings] ProxyCount: %d\n", proxyCount);
                
                for (int i = 0; i < proxyCount; i++) {
                    ProxyData proxy;
                    int32_t type;
                    std::string host, user, password;
                    uint32_t port;
                    
                    if (!reader.readInt32(type)) return false;
                    if (!reader.readString(host)) return false;
                    if (!reader.readUint32(port)) return false;
                    if (!reader.readString(user)) return false;
                    if (!reader.readString(password)) return false;
                    
                    proxy.type = (ProxyType)type;
                    proxy.host = host;
                    proxy.port = port;
                    proxy.user = user;
                    proxy.password = password;
                    
                    m_settings.proxies.push_back(proxy);
                    
                    printf("    [Proxy %d] Type: %d, Host: %s, Port: %u\n", 
                           i, type, host.c_str(), port);
                }
                
                // Read selected proxy index
                int32_t selected;
                if (!reader.readInt32(selected)) return false;
                m_settings.selectedProxy = selected;
                printf("  [Settings] SelectedProxy: %d\n", selected);
            }
            break;
        }
        
        case dbiFallbackProductionConfig: {
            std::vector<uint8_t> config;
            if (!reader.readByteArray(config)) return false;
            printf("  [Settings] FallbackProductionConfig: %u bytes\n", (unsigned)config.size());
            break;
        }
        
        case dbiApplicationSettings: {
            std::vector<uint8_t> appSettings;
            if (!reader.readByteArray(appSettings)) return false;
            printf("  [Settings] ApplicationSettings: %u bytes\n", (unsigned)appSettings.size());
            // Parse archive settings from application settings if present
            break;
        }
        
        case dbiBackgroundKey: {
            uint64_t keyDay, keyNight;
            if (!reader.readUint64(keyDay)) return false;
            if (!reader.readUint64(keyNight)) return false;
            printf("  [Settings] BackgroundKeyDay: 0x%llx, BackgroundKeyNight: 0x%llx\n", 
                   (unsigned long long)keyDay, (unsigned long long)keyNight);
            break;
        }
        
        case dbiTileBackground: {
            int32_t tileDay, tileNight;
            if (!reader.readInt32(tileDay)) return false;
            if (!reader.readInt32(tileNight)) return false;
            printf("  [Settings] TileBackgroundDay: %d, TileBackgroundNight: %d\n", tileDay, tileNight);
            break;
        }
        
        default:
            printf("  [Settings] Unknown block: 0x%x\n", blockId);
            break;
    }
    
    return true;
}

bool TGSettingsParser::setProxy(const ProxyData& proxy) {
    if (m_settings.proxies.empty()) {
        m_settings.proxies.push_back(proxy);
        m_settings.selectedProxy = 0;
    } else if (m_settings.selectedProxy >= 0 && m_settings.selectedProxy < (int)m_settings.proxies.size()) {
        m_settings.proxies[m_settings.selectedProxy] = proxy;
    } else {
        m_settings.proxies.push_back(proxy);
        m_settings.selectedProxy = (int)m_settings.proxies.size() - 1;
    }
    m_settings.proxySettings = ProxySettings_Enabled;
    return true;
}

bool TGSettingsParser::addProxy(const ProxyData& proxy) {
    m_settings.proxies.push_back(proxy);
    return true;
}

bool TGSettingsParser::removeProxy(int index) {
    if (index < 0 || index >= (int)m_settings.proxies.size()) {
        m_error = "Invalid proxy index";
        return false;
    }
    m_settings.proxies.erase(m_settings.proxies.begin() + index);
    if (m_settings.selectedProxy >= (int)m_settings.proxies.size()) {
        m_settings.selectedProxy = (int)m_settings.proxies.size() - 1;
    }
    return true;
}

bool TGSettingsParser::setProxySettings(ProxySettings settings) {
    m_settings.proxySettings = settings;
    return true;
}

bool TGSettingsParser::selectProxy(int index) {
    if (index < -1 || index >= (int)m_settings.proxies.size()) {
        m_error = "Invalid proxy index";
        return false;
    }
    m_settings.selectedProxy = index;
    return true;
}

bool TGSettingsParser::setArchiveCollapsed(bool collapsed) {
    m_settings.archive.archiveCollapsed = collapsed;
    return true;
}

bool TGSettingsParser::setArchiveInMainMenu(bool inMainMenu) {
    m_settings.archive.archiveInMainMenu = inMainMenu;
    return true;
}

void TGSettingsParser::serializeSettings(std::vector<uint8_t>& data) {
    DataWriter writer;
    
    // Write basic settings
    writer.writeUint32(dbiAutoStart);
    writer.writeInt32(m_settings.autoStart);
    
    writer.writeUint32(dbiStartMinimized);
    writer.writeInt32(m_settings.startMinimized);
    
    writer.writeUint32(dbiSendToMenu);
    writer.writeInt32(m_settings.sendToMenu);
    
    writer.writeUint32(dbiSeenTrayTooltip);
    writer.writeInt32(m_settings.seenTrayTooltip);
    
    writer.writeUint32(dbiAutoUpdate);
    writer.writeInt32(m_settings.autoUpdate);
    
    writer.writeUint32(dbiLastUpdateCheck);
    writer.writeInt32(m_settings.lastUpdateCheck);
    
    writer.writeUint32(dbiScalePercent);
    writer.writeInt32(m_settings.scalePercent);
    
    writer.writeUint32(dbiDialogLastPath);
    writer.writeString(m_settings.dialogLastPath);
    
    writer.writeUint32(dbiPowerSaving);
    writer.writeInt32(m_settings.powerSaving);
    
    // Write theme keys
    writer.writeUint32(dbiThemeKey);
    writer.writeUint64(m_settings.themeKeyDay);
    writer.writeUint64(m_settings.themeKeyNight);
    writer.writeUint32(m_settings.nightMode);
    
    // Write language keys
    if (m_settings.langPackKey != 0) {
        writer.writeUint32(dbiLangPackKey);
        writer.writeUint64(m_settings.langPackKey);
    }
    
    if (m_settings.languagesKey != 0) {
        writer.writeUint32(dbiLanguagesKey);
        writer.writeUint64(m_settings.languagesKey);
    }
    
    // Write proxy settings
    writer.writeUint32(dbiConnectionType);
    writer.writeInt32((int32_t)m_settings.proxySettings);
    
    if (m_settings.proxySettings == ProxySettings_Enabled && !m_settings.proxies.empty()) {
        writer.writeInt32((int32_t)m_settings.proxies.size());
        
        for (size_t i = 0; i < m_settings.proxies.size(); i++) {
            const ProxyData& proxy = m_settings.proxies[i];
            writer.writeInt32((int32_t)proxy.type);
            writer.writeString(proxy.host);
            writer.writeUint32(proxy.port);
            writer.writeString(proxy.user);
            writer.writeString(proxy.password);
        }
        
        writer.writeInt32(m_settings.selectedProxy);
    }
    
    data = writer.getData();
}

bool TGSettingsParser::saveSettings(const std::string& tdataPath) {
    if (!m_keyCreated) {
        m_error = "Settings not loaded, cannot save";
        return false;
    }
    
    // Serialize settings
    std::vector<uint8_t> settingsData;
    serializeSettings(settingsData);
    
    printf("[+] Serialized settings: %u bytes\n", (unsigned)settingsData.size());
    
    // Encrypt settings
    std::vector<uint8_t> encrypted;
    if (!encryptData(settingsData, encrypted)) {
        return false;
    }
    
    printf("[+] Encrypted settings: %u bytes\n", (unsigned)encrypted.size());
    
    // Build output data
    DataWriter fileWriter;
    fileWriter.writeByteArray(m_salt);
    fileWriter.writeByteArray(encrypted);
    
    std::vector<uint8_t>& fileData = fileWriter.getData();
    
    // Calculate MD5
    int32_t dataSize = (int32_t)fileData.size();
    uint8_t md5Result[16];
    
    TGCrypto::md5Init();
    TGCrypto::md5Update(&fileData[0], fileData.size());
    TGCrypto::md5Update(&dataSize, sizeof(dataSize));
    TGCrypto::md5Update(&m_version, sizeof(m_version));
    TGCrypto::md5Update(TDF_MAGIC, TDF_MAGIC_LEN);
    TGCrypto::md5Final(md5Result);
    
    // Append MD5
    fileData.insert(fileData.end(), md5Result, md5Result + 16);
    
    // Build final file with magic and version
    std::vector<uint8_t> finalData;
    finalData.insert(finalData.end(), TDF_MAGIC, TDF_MAGIC + TDF_MAGIC_LEN);
    finalData.insert(finalData.end(), (uint8_t*)&m_version, (uint8_t*)&m_version + 4);
    finalData.insert(finalData.end(), fileData.begin(), fileData.end());
    
    // Build output path
    std::string settingsPath = tdataPath;
    if (!settingsPath.empty() && settingsPath[settingsPath.size()-1] != PATH_SEPARATOR) {
        settingsPath += PATH_SEPARATOR;
    }
    settingsPath += "settings";
    
    // Write to settingss (modern format)
    std::string outputPath = settingsPath + "s";
    if (!writeFile(outputPath, finalData)) {
        return false;
    }
    
    printf("[+] Settings saved to: %s\n", outputPath.c_str());
    return true;
}

void TGSettingsParser::printSettings() const {
    printf("\n=== Telegram Desktop Settings ===\n");
    printf("AutoStart: %d\n", m_settings.autoStart);
    printf("StartMinimized: %d\n", m_settings.startMinimized);
    printf("SendToMenu: %d\n", m_settings.sendToMenu);
    printf("AutoUpdate: %d\n", m_settings.autoUpdate);
    printf("ScalePercent: %d%%\n", m_settings.scalePercent);
    printf("NightMode: %d\n", m_settings.nightMode);
    printf("PowerSaving: %d\n", m_settings.powerSaving);
    printf("DialogLastPath: %s\n", m_settings.dialogLastPath.c_str());
    printf("\n");
}

void TGSettingsParser::printProxies() const {
    printf("\n=== Proxy Configuration ===\n");
    printf("Proxy Settings: ");
    switch (m_settings.proxySettings) {
        case ProxySettings_System: printf("System\n"); break;
        case ProxySettings_Enabled: printf("Enabled\n"); break;
        case ProxySettings_Disabled: printf("Disabled\n"); break;
    }
    printf("Selected Proxy: %d\n", m_settings.selectedProxy);
    printf("Proxy Count: %d\n", (int)m_settings.proxies.size());
    
    for (size_t i = 0; i < m_settings.proxies.size(); i++) {
        const ProxyData& proxy = m_settings.proxies[i];
        printf("\n[Proxy %d]\n", (int)i);
        printf("  Type: ");
        switch (proxy.type) {
            case ProxyType_None: printf("None\n"); break;
            case ProxyType_Socks5: printf("SOCKS5\n"); break;
            case ProxyType_Http: printf("HTTP\n"); break;
            case ProxyType_Mtproto: printf("MTProto\n"); break;
        }
        printf("  Host: %s\n", proxy.host.c_str());
        printf("  Port: %u\n", proxy.port);
        if (!proxy.user.empty()) {
            printf("  User: %s\n", proxy.user.c_str());
        }
        if (!proxy.password.empty()) {
            printf("  Password: %s\n", proxy.password.c_str());
        }
    }
    printf("\n");
}

void TGSettingsParser::printArchiveSettings() const {
    printf("\n=== Archive Settings ===\n");
    printf("Archive Collapsed: %s\n", m_settings.archive.archiveCollapsed ? "Yes" : "No");
    printf("Archive In Main Menu: %s\n", m_settings.archive.archiveInMainMenu ? "Yes" : "No");
    printf("\n");
}
