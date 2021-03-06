/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/nb_acl.h"

namespace necbaas {

using std::string;
using std::list;

// コンストラクタ
NbAcl::NbAcl() {}

NbAcl::NbAcl(const NbJsonObject &json) {
    SetJson(json);
}

// デストラクタ.
NbAcl::~NbAcl() {}

const list<string> &NbAcl::GetAdmin() const {
    return admin_;
}

const string &NbAcl::GetOwner() const {
    return owner_;
}

void NbAcl::SetAdmin(const list<string> &admin) {
    SetEntries(admin, &admin_);
}

void NbAcl::SetOwner(const string &owner) {
    owner_ = owner;
}

list<string> &NbAcl::GetListByPermission(NbAclPermission permission) {
    switch (permission) {
        case NbAclPermission::ADMIN:
            return admin_;
        default:
            return NbAclBase::GetListByPermission(permission);
    }
}

void NbAcl::SetJson(const NbJsonObject &json) {
    owner_ = json.GetString(kKeyOwner);
    json.GetJsonArray(kKeyAdmin).GetAllString(&admin_);
    SortUnique(&admin_);
    NbAclBase::SetJson(json);
}

NbJsonObject NbAcl::ToJsonObject() const {
    NbJsonObject json = NbAclBase::ToJsonObject();
    NbJsonArray array;

    // オーナは空文字の場合は設定しない
    if (!owner_.empty()) {
        json[kKeyOwner] = owner_;
    }

    array.PutAllList(admin_);
    json.PutJsonArray(kKeyAdmin, array);

    return json;
}

NbAcl NbAcl::CreateAclForAnonymous() {
    return CreateAclFor(kGroupAnonymous);
}

NbAcl NbAcl::CreateAclForAuthenticated() {
    return CreateAclFor(kGroupAuthenticated);
}

NbAcl NbAcl::CreateAclForUser(const NbUser &user) {
    NbAcl acl = CreateAclFor(user.GetUserId());
    acl.owner_ = user.GetUserId();
    return acl;
}

NbAcl NbAcl::CreateAclFor(const string &entry) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::READ, entry);
    acl.AddEntry(NbAclPermission::WRITE, entry);
    acl.AddEntry(NbAclPermission::ADMIN, entry);
    return acl;
}

NbAcl NbAcl::CreateAclFor(const list<string> &entries) {
    NbAcl acl;
    acl.SetRead(entries);
    acl.SetWrite(entries);
    acl.SetAdmin(entries);
    return acl;
}
} //namespace necbaas
