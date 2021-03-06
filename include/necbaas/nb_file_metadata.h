/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBFILEMETADATA_H
#define NECBAAS_NBFILEMETADATA_H

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "necbaas/nb_json_object.h"
#include "necbaas/nb_acl.h"

namespace necbaas {

/**
 * @class NbFileMetadata nb_file_metadata.h "necbaas/nb_file_metadata.h"
 * ファイルメタデータ.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbFileMetadata {
   public:
    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>コンストラクタ.</p>
     */
    NbFileMetadata();

    /**
     * コンストラクタ.
     * Jsonオブジェクトからファイルメタデータを構築する。
     * @param[in]   bucket_name     バケット名
     * @param[in]   json            Jsonオブジェクト
     */
    NbFileMetadata(const std::string &bucket_name, const NbJsonObject &json);

    /**
     * デストラクタ.
     */
    ~NbFileMetadata();

    /**
     * ファイル名取得.
     * @return      ファイル名
     */
    const std::string &GetFileName() const;

    /**
     * Content-Type取得.
     * @return      Content-Type
     */
    const std::string &GetContentType() const;

    /**
     * ACL取得.
     * @return      ACL
     */
    const NbAcl &GetAcl() const;

    /**
     * ファイルサイズ取得.
     * @return      ファイルサイズ
     */
    int GetLength() const;

    /**
     * ファイル作成日時取得.
     * std::tm の tm_year が 0 の場合は無効データ。
     * @return      ファイル作成日時(UTC)
     */
    std::tm GetCreatedTime() const;

    /**
     * ファイル更新日時取得.
     * std::tm の tm_year が 0 の場合は無効データ。
     * @return      ファイル更新日時(UTC)
     */
    std::tm GetUpdatedTime() const;

    /**
     * メタデータのETag取得.
     * @return      メタデータのETag
     */
    const std::string &GetMetaETag() const;

    /**
     * ファイル本体のETag取得.
     * @return      ファイル本体のETag
     */
    const std::string &GetFileETag() const;

    /**
     * キャッシュ禁止状態取得.
     * @return          キャッシュ禁止状態
     * @retval  true    キャッシュ禁止
     * @retval  false   キャッシュ許可
     */
    bool IsCacheDisabled() const;

    /**
     * 論理削除状態取得.
     * @return          論理削除状態
     * @retval  true    論理削除
     * @retval  false   論理削除されていない
     */
    bool IsDeleted() const;

   private:
    std::string file_name_;      /*!< ファイル名 */
    std::string content_type_;   /*!< Content-Type */
    NbAcl acl_;                  /*!< ACL */
    int length_{0};              /*!< ファイルサイズ */
    std::string created_time_;   /*!< 作成日時 */
    std::string updated_time_;   /*!< 更新日時 */
    std::string meta_etag_;      /*!< メタデータのETag */
    std::string file_etag_;      /*!< ファイル本体のETag */
    bool cache_disabled_{false}; /*!< キャッシュ禁止フラグ */
    bool deleted_{false};        /*!< 削除マーク */

    std::string parent_bucket_name_; /*!< バケット名 */
};
}  // namespace necbaas
#endif  // NECBAAS_NBFILEMETADATA_H
