#ifndef __IOPENCDMI_H
#define __IOPENCDMI_H

#include <core/core.h>
#include <com/com.h>

namespace OCDM {

typedef int32_t OCDM_RESULT;

// ISession defines the interface towards a DRM context that can decrypt data
// using a given key.
struct ISession : virtual public WPEFramework::Core::IUnknown {
    enum KeyStatus {
        Usable = 0,
        Expired,
        Released,
        OutputRestricted,
        OutputDownscaled,
        StatusPending,
        InternalError
    };

    // ICallback defines the callback interface to receive
    // events originated from the session.
    struct ICallback : virtual public WPEFramework::Core::IUnknown {
        enum { ID = WPEFramework::RPC::ID_SESSION_CALLBACK };

        virtual ~ICallback() {}

        // Event fired when a key message is successfully created.
        virtual void OnKeyMessage(
            const uint8_t* keyMessage, //__in_bcount(f_cbKeyMessage)
            const uint16_t keyLength, //__in
            const std::string URL)
            = 0; //__in_z_opt

        // Event fired when MediaKeySession has found a usable key.
        virtual void OnKeyReady() = 0;

        // Event fired when MediaKeySession encounters an error.
        virtual void OnKeyError(
            const int16_t error,
            const OCDM_RESULT sysError,
            const std::string errorMessage)
            = 0;

        // Event fired on key status update
        virtual void OnKeyStatusUpdate(const ISession::KeyStatus status) = 0;
    };

    enum { ID = WPEFramework::RPC::ID_SESSION };

    virtual ~ISession(void) {}

    // Loads the data stored for the specified session into the cdm object
    virtual OCDM_RESULT Load() = 0;

    // Process a key message response.
    virtual void Update(
        const uint8_t* keyMessage, //__in_bcount(f_cbKeyMessageResponse)
        const uint16_t keyLength)
        = 0; //__in

    //Removes all license(s) and key(s) associated with the session
    virtual OCDM_RESULT Remove() = 0;

    //Report the current status of the Session with respect to the KeyExchange.
    virtual KeyStatus Status() const = 0;

    //Report the name to be used for the Shared Memory for exchanging the Encrypted fragements.
    virtual std::string BufferId() const = 0;

    //Report the name to be used for the Shared Memory for exchanging the Encrypted fragements.
    virtual std::string SessionId() const = 0;

    //We are completely done with the session, it can be closed.
    virtual void Close() = 0;

    //During instantiation a callback is set, here we can decouple.
    virtual void Revoke(OCDM::ISession::ICallback* callback) = 0;
};

    // TODO: should derive from ISession?
    struct ISessionExt : virtual public WPEFramework::Core::IUnknown
    {
        enum { ID = 0x00000072 };

        enum LicenseTypeExt {
            Invalid = 0,
            LimitedDuration,
            Standard
        };

        enum SessionStateExt {
            LicenseAcquisitionState = 0,
            InactiveDecryptionState,
            ActiveDecryptionState,
            InvalidState
        };

        virtual uint32_t SessionIdExt() const = 0;

        //Report the name to be used for the Shared Memory for exchanging the Encrypted fragements.
        virtual std::string BufferIdExt() const = 0;

        virtual uint16_t PlaylevelCompressedVideo() const = 0;
        virtual uint16_t PlaylevelUncompressedVideo() const = 0;
        virtual uint16_t PlaylevelAnalogVideo() const = 0;
        virtual uint16_t PlaylevelCompressedAudio() const = 0;
        virtual uint16_t PlaylevelUncompressedAudio() const = 0;

        virtual std::string GetContentIdExt() const = 0;
        virtual void SetContentIdExt(const std::string & contentId) = 0;

        virtual LicenseTypeExt GetLicenseTypeExt() const = 0;
        virtual void SetLicenseTypeExt(LicenseTypeExt licenseType) = 0;

        virtual SessionStateExt GetSessionStateExt() const = 0;
        virtual void SetSessionStateExt(SessionStateExt sessionState) = 0;

        virtual OCDM_RESULT SetDrmHeader(const uint8_t drmHeader[], uint32_t drmHeaderLength) = 0;

        virtual OCDM_RESULT GetChallengeDataNetflix(uint8_t * challenge, uint32_t & challengeSize, uint32_t isLDL) = 0;

        virtual OCDM_RESULT StoreLicenseData(const uint8_t licenseData[], uint32_t licenseDataSize, unsigned char * secureStopId) = 0;

        virtual OCDM_RESULT InitDecryptContextByKid() = 0;
    };

struct IAccessorOCDM : virtual public WPEFramework::Core::IUnknown {

    enum { ID = WPEFramework::RPC::ID_ACCESSOROCDM };

    struct INotification : virtual public WPEFramework::Core::IUnknown {

        enum { ID = WPEFramework::RPC::ID_ACCESSOROCDM_NOTIFICATION };

        virtual ~INotification() {}

        virtual void Create(const string& sessionId) = 0;
        virtual void Destroy(const string& sessionId) = 0;
        virtual void KeyChange(const string& sessionId, const uint8_t keyId[], const uint8_t length, const OCDM::ISession::KeyStatus status) = 0;
   };

    virtual ~IAccessorOCDM() {}

    virtual OCDM::OCDM_RESULT IsTypeSupported(
        const std::string keySystem,
        const std::string mimeType) const = 0;

    // Create a MediaKeySession using the supplied init data and CDM data.
    virtual OCDM_RESULT CreateSession(
        const string keySystem,
        const int32_t licenseType,
        const std::string initDataType,
        const uint8_t* initData,
        const uint16_t initDataLength,
        const uint8_t* CDMData,
        const uint16_t CDMDataLength,
        ISession::ICallback* callback,
        std::string& sessionId,
        ISession*& session)
        = 0;

    // Set Server Certificate
    virtual OCDM_RESULT SetServerCertificate(
        const string keySystem,
        const uint8_t* serverCertificate,
        const uint16_t serverCertificateLength)
        = 0;

    virtual void Register(
        INotification* sink)
        = 0;

    virtual void Unregister(
        INotification* sink)
        = 0;

    virtual ISession* Session(
        const std::string sessionId)
        = 0;

    virtual ISession* Session(
        const uint8_t keyId[], const uint8_t length)
        = 0;
};
    struct IAccessorOCDMExt : virtual public WPEFramework::Core::IUnknown {

        enum { ID = 0x00000071 };

        virtual time_t GetDrmSystemTime() const = 0;

        virtual OCDM_RESULT CreateSessionExt(
            uint32_t sessionId,
            const char contentId[],
            uint32_t contentIdLength,
            ISessionExt::LicenseTypeExt licenseType,
            const uint8_t drmHeader[],
            uint32_t drmHeaderLength,
            ISessionExt*& session) = 0;

        virtual std::string GetVersionExt() const = 0;

        virtual uint32_t GetLdlSessionLimit() const = 0;

        virtual OCDM_RESULT EnableSecureStop(bool enable) = 0;

        virtual OCDM_RESULT CommitSecureStop(
                const unsigned char sessionID[],
                uint32_t sessionIDLength,
                const unsigned char serverResponse[],
                uint32_t serverResponseLength) = 0;

        // TODO: rename to something like "SetStoreDirs"
        virtual OCDM_RESULT CreateSystemNetflix(
                const std::string& readDir,
                const std::string& storeLocation) = 0;

        virtual OCDM_RESULT InitSystemNetflix() = 0;

        virtual OCDM_RESULT TeardownSystemNetflix() = 0;

        virtual OCDM_RESULT DeleteSecureStore() = 0;

        virtual OCDM_RESULT GetSecureStoreHash(
                uint8_t secureStoreHash[],
                uint32_t secureStoreHashLength) = 0;
    };
}

#endif // __OPENCDMI_
