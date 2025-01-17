#ifndef PROVIDERUDPSSL_H
#define PROVIDERUDPSSL_H

#include <leddevice/LedDevice.h>
#include <utils/Logger.h>

// Qt includes
#include <QMutex>
#include <QMutexLocker>
#include <QHostInfo>
#include <QThread>

//----------- mbedtls
#if defined(HYPERHDR_MBEDTLS_VER3)
#include <mbedtls/build_info.h>
#elif !defined(MBEDTLS_CONFIG_FILE)
#include <mbedtls/config.h>
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include <mbedtls/platform.h>
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_time         time
#define mbedtls_time_t       time_t
#define mbedtls_printf       printf
#define mbedtls_fprintf      fprintf
#define mbedtls_snprintf     snprintf
#define mbedtls_calloc       calloc
#define mbedtls_free         free
#define mbedtls_exit         exit
#define MBEDTLS_EXIT_SUCCESS EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE EXIT_FAILURE
#endif

#include <string.h>
#include <cstring>
#include <chrono>

#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl_ciphersuites.h>
#include <mbedtls/entropy.h>
#include <mbedtls/timing.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/debug.h>

//----------- END mbedtls

constexpr std::chrono::milliseconds STREAM_SSL_HANDSHAKE_TIMEOUT_MIN{400};
constexpr std::chrono::milliseconds STREAM_SSL_HANDSHAKE_TIMEOUT_MAX{1000};
constexpr std::chrono::milliseconds STREAM_SSL_READ_TIMEOUT{0};

class ProviderUdpSSL : public LedDevice
{
	Q_OBJECT

public:
	///
	/// @brief Constructs an UDP SSL LED-device
	///
	ProviderUdpSSL(const QJsonObject &deviceConfig);

	///
	/// @brief Destructor of the LED-device
	///
	~ProviderUdpSSL() override;

protected:

	///
	/// @brief Initialise the UDP-SSL device's configuration and network address details
	///
	/// @param[in] deviceConfig the JSON device configuration
	/// @return True, if success#endif // PROVIDERUDP_H
	///
	bool init(const QJsonObject &deviceConfig) override;

	///
	/// @brief Opens the output device.
	///
	/// @return Zero on success (i.e. device is ready), else negative
	///
	int open() override;

	///
	/// @brief Closes the output device.
	///
	/// @return Zero on success (i.e. device is closed), else negative
	///
	int close() override;

	///
	/// @brief Initialise device's network details
	///
	/// @return True, if success
	///
	bool initNetwork();

	///
	/// Writes the given bytes/bits to the UDP-device and sleeps the latch time to ensure that the
	/// values are latched.
	///
	/// @param[in] size The length of the data
	/// @param[in] data The data
	///
	void writeBytes(unsigned int size, const uint8_t *data);

	///
	/// get ciphersuites list from mbedtls_ssl_list_ciphersuites
	///
	/// @return const int * array
	///
	virtual const int * getCiphersuites() const;

	void sslLog(const QString &msg, const char* errorType = "debug");
	void sslLog(const char* msg, const char* errorType = "debug");
	void configLog(const char* msg, const char* type, ...);

	/**
	 * Debug callback for mbed TLS
	 * Just prints on the USB serial port
	 */
	static void ProviderUdpSSLDebug(void* ctx, int level, const char* file, int line, const char* str);

	/**
	 * Certificate verification callback for mbed TLS
	 * Here we only use it to display information on each cert in the chain
	 */
	static int ProviderUdpSSLVerify(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags);

	///
	/// closeSSLNotify and freeSSLConnection
	///
	void closeSSLConnection();

private:

	bool initConnection();
	bool seedingRNG();
	bool setupStructure();
	bool startUPDConnection();
	bool setupPSK();
	bool startSSLHandshake();
	void handleReturn(int ret);
	QString errorMsg(int ret);
	void closeSSLNotify();
	void freeSSLConnection();

	mbedtls_net_context          client_fd;
	mbedtls_entropy_context      entropy;
	mbedtls_ssl_context          ssl;
	mbedtls_ssl_config           conf;
	mbedtls_x509_crt             cacert;
	mbedtls_ctr_drbg_context     ctr_drbg;
	mbedtls_timing_delay_context timer;

	QMutex       _hueMutex;
	QString      _transport_type;
	QString      _custom;
	QHostAddress _address;
	QString      _defaultHost;
	int          _port;
	int          _ssl_port;
	QString      _server_name;
	QString      _psk;
	QString      _psk_identity;
	uint32_t     _read_timeout;
	uint32_t     _handshake_timeout_min;
	uint32_t     _handshake_timeout_max;
	unsigned int _handshake_attempts;
	int          _retry_left;
	bool         _stopConnection;
	bool         _debugStreamer;
	int          _debugLevel;
};

#endif // PROVIDERUDPSSL_H
