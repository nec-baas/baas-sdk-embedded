/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/nb_object_bucket.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;
using std::multimap;

// コンストラクタ
NbObjectBucket::NbObjectBucket(const std::shared_ptr<NbService> &service, const std::string &bucket_name)
        : service_(service), bucket_name_(bucket_name) {}

NbObjectBucket::~NbObjectBucket() {}

NbResult<NbObject> NbObjectBucket::GetObject(const string &object_id, bool delete_mark) {
    NBLOG(TRACE) << __func__;

    NbResult<NbObject> result;

    if (object_id.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_INVALID_ARGUMENT);
        NBLOG(ERROR) << "Object ID is empty.";
        return result;
    }

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    NbResult<NbHttpResponse> rest_result = service_->ExecuteRequest(
        [this, &object_id, delete_mark](NbHttpRequestFactory &request_factory) -> NbHttpRequest {
            request_factory.Get(kObjectsPath)
                           .AppendPath("/" + bucket_name_ + "/" + object_id);
            if (delete_mark) {
                request_factory.AppendParam(kKeyDeleteMark, "1");
            }
            return request_factory.Build();
        }, timeout_);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json(http_response.GetBody());
        NbObject object(service_, bucket_name_);
        object.SetCurrentParam(json);
        result.SetSuccessData(object);
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

NbResult<vector<NbObject>> NbObjectBucket::Query(const NbQuery &query, int *count) {
    NBLOG(TRACE) << __func__;

    NbResult<vector<NbObject>> result;

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    NbResult<NbHttpResponse> rest_result = service_->ExecuteRequest(
        [this, &query, count](NbHttpRequestFactory &request_factory) -> NbHttpRequest {
            return request_factory.Get(kObjectsPath)
                           .AppendPath("/" + bucket_name_)
                           .Params(GetParams(query, count))
                           .Build();
        }, timeout_);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json(http_response.GetBody());

        //"results"配列型を取得
        NbJsonArray query_results_array = json.GetJsonArray(kKeyResults);

        NbObject object(service_, bucket_name_);
        vector<NbObject> vector_object;
        for (int i = 0; i < query_results_array.GetSize(); i++) {
            //results"配列からJson Objectを取り出し、NbObjectの配列を構築
            NbJsonObject query_result =  query_results_array.GetJsonObject(i);
            object.SetCurrentParam(query_result);
            vector_object.push_back(object);
        }
        result.SetSuccessData(vector_object);

        if (count) {
            *count = json.GetInt(kKeyCount);
        }
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

multimap<string, string> NbObjectBucket::GetParams(const NbQuery &query, int *count) const {
    multimap<string, string> params;

    string param_string = query.GetConditionsString();
    if (!param_string.empty()) {
        params.insert(std::make_pair(kKeyWhere, param_string));
    }
    param_string = query.GetOrderString();
    if (!param_string.empty()) {
        params.insert(std::make_pair(kKeyOrder, param_string));
    }
    param_string = query.GetSkipString();
    if (!param_string.empty()) {
        params.insert(std::make_pair(kKeySkip, param_string));
    }
    param_string = query.GetLimitString();
    if (!param_string.empty()) {
        params.insert(std::make_pair(kKeyLimit, param_string));
    }
    param_string = query.GetDeleteMarkString();
    if (!param_string.empty()) {
        params.insert(std::make_pair(kKeyDeleteMark, param_string));
    }
    param_string = query.GetProjectionString();
    if (!param_string.empty()) {
        params.insert(std::make_pair(kKeyProjection, param_string));
    }
    param_string = query.GetReadPreferenceString();
    if (!param_string.empty()) {
        params.insert(std::make_pair(kKeyReadPreference, param_string));
    }
    param_string = query.GetTimeoutString();
    if (!param_string.empty()) {
        params.insert(std::make_pair(kKeyTimeout, param_string));
    }
    if (count) {
        params.insert(std::make_pair(kKeyCount, string("1")));
    }
    return params;
}

int NbObjectBucket::GetTimeout() const {
    return timeout_;
}

void NbObjectBucket::SetTimeout(int timeout) {
    timeout_ = timeout;
}

NbObject NbObjectBucket::NewObject() const {
    NbObject object(service_, bucket_name_);
    return object;
}

const string &NbObjectBucket::GetBucketName() const {
    return bucket_name_;
}
} //namespace necbaas
