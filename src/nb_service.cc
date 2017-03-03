/*
 * COPYRIGHT (C) 2017 NEC CORPORATION
 *
 * ALL RIGHTS RESERVED BY NEC CORPORATION,  THIS PROGRAM
 * MUST BE USED SOLELY FOR THE PURPOSE FOR WHICH IT WAS
 * FURNISHED BY NEC CORPORATION,  NO PART OF THIS PROGRAM
 * MAY BE REPRODUCED OR DISCLOSED TO OTHERS,  IN ANY FORM
 * WITHOUT THE PRIOR WRITTEN PERMISSION OF NEC CORPORATION.
 *
 * NEC CONFIDENTIAL AND PROPRIETARY
 */

#include "necbaas/nb_service.h"
#include <mutex>
#include <curlpp/cURLpp.hpp>
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::shared_ptr;

static std::mutex mutex_curl;

// サービス生成
shared_ptr<NbService> NbService::CreateService(const string &endpoint_url, const string &tenant_id,
                                               const string &app_id, const string &app_key, const string &proxy) {
    NBLOG(DEBUG) << "NbService::CreateService";
    return shared_ptr<NbService>(new NbService(endpoint_url, tenant_id, app_id, app_key, proxy));
}

void NbService::SetDebugLogEnabled(bool flag) {
    NbLogger::SetDebugLogEnabled(flag);
}

void NbService::SetErrorLogEnabled(bool flag) {
    NbLogger::SetErrorLogEnabled(flag);
}

void NbService::SetRestLogEnabled(bool flag) {
    NbLogger::SetRestLogEnabled(flag);
}

// コンストラクタ
NbService::NbService(const string &endpoint_url, const string &tenant_id, const string &app_id,
                     const string &app_key, const string &proxy)
        : endpoint_url_(endpoint_url), tenant_id_(tenant_id), app_id_(app_id), app_key_(app_key), proxy_(proxy) {
    // curl_global_init()がスレッドセーフでないため、排他する
    std::lock_guard<std::mutex> lock(mutex_curl);
    curlpp::initialize();
}

// デストラクタ
NbService::~NbService() {
    // curl_global_cleanup()がスレッドセーフでないため、排他する
    std::lock_guard<std::mutex> lock(mutex_curl);
    curlpp::terminate();
}

// Getter
const string &NbService::GetAppId() const {
    return app_id_;
}

const string &NbService::GetAppKey() const {
    return app_key_;
}

const string &NbService::GetEndpointUrl() const {
    return endpoint_url_;
}

const string &NbService::GetTenantId() const {
    return tenant_id_;
}

const string &NbService::GetProxy() const {
    return proxy_;
}

NbSessionToken NbService::GetSessionToken() {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    return session_token_;
}
void NbService::SetSessionToken(NbSessionToken token) {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    session_token_ = token;
}

void NbService::ClearSessionToken() {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    session_token_.ClearSessionToken();
}

NbRestExecutor *NbService::PopRestExecutor() {
    return rest_executor_pool_.PopRestExecutor();
}

void NbService::PushRestExecutor(NbRestExecutor *executor) {
    rest_executor_pool_.PushRestExecutor(executor);
}

NbHttpRequestFactory NbService::GetHttpRequestFactory() {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    return NbHttpRequestFactory(endpoint_url_, tenant_id_, app_id_, app_key_,
                                session_token_.GetSessionToken(), proxy_);
}
} //namespace necbaas