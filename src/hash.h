#ifndef __hash_table_definition_included__
#define __hash_table_definition_included__

static const unsigned attr_hash_table[] = {
    ~0,
    ~0,
    BBAMI_APPLICATION_REQUIRES_SYSTEM     , /* 2       Application-Requires-System */
    BBAMI_PACKAGE_AUTHOR_CERTIFICATE_HASH , /* 3       Package-Author-Certificate-Hash */
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    BBAMI_ENTRY_POINT_ICON                , /* 10      Entry-Point-Icon */
    ~0,
    ~0,
    BBAMI_ENTRY_POINT_NAME                , /* 13      Entry-Point-Name */
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    BBAMI_APPLICATION_DEVELOPMENT_MODE    , /* 19      Application-Development-Mode */
    ~0,
    ~0,
    ~0,
    BBAMI_APPLICATION_VERSION             , /* 23      Application-Version */
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    BBAMI_ENTRY_POINT                     , /* 30      Entry-Point */
    ~0,
    ~0,
    ~0,
    ~0,
    BBAMI_PACKAGE_VERSION                 , /* 35      Package-Version */
    ~0,
    ~0,
    ~0,
    BBAMI_AUTHOR                          , /* 39      Author */
    BBAMI_APPLICATION_ID                  , /* 40      Application-Id */
    BBAMI_PACKAGE_AUTHOR_ID               , /* 41      Package-Author-Id */
    ~0,
    ~0,
    BBAMI_ARCHIVE_MANIFEST_VERSION        , /* 44      Archive-Manifest-Version */
    ~0,
    ~0,
    BBAMI_ENTRY_POINT_SYSTEM_ACTIONS      , /* 47      Entry-Point-System-Actions */
    ~0,
    BBAMI_ENTRY_POINT_TYPE                , /* 49      Entry-Point-Type */
    BBAMI_ENTRY_POINT_SPLASH_SCREEN       , /* 50      Entry-Point-Splash-Screen */
    ~0,
    BBAMI_PACKAGE_VERSION_ID              , /* 52      Package-Version-Id */
    ~0,
    ~0,
    ~0,
    ~0,
    BBAMI_APPLICATION_DESCRIPTION         , /* 57      Application-Description */
    ~0,
    ~0,
    ~0,
    BBAMI_APPLICATION_VERSION_ID          , /* 61      Application-Version-Id */
    BBAMI_PACKAGE_ID                      , /* 62      Package-Id */
    ~0,
    ~0,
    BBAMI_ARCHIVE_ASSET_TYPE              , /* 65      Archive-Asset-Type */
    BBAMI_ENTRY_POINT_ORIENTATION         , /* 66      Entry-Point-Orientation */
    ~0,
    BBAMI_ARCHIVE_CREATED_BY              , /* 68      Archive-Created-By */
    BBAMI_ENTRY_POINT_USER_ACTIONS        , /* 69      Entry-Point-User-Actions */
    ~0,
    ~0,
    ~0,
    BBAMI_PACKAGE_ARCHITECTURE            , /* 73      Package-Architecture */
    ~0,
    BBAMI_PACKAGE_AUTHOR                  , /* 75      Package-Author */
    BBAMI_APPLICATION_NAME                , /* 76      Application-Name */
    ~0,
    ~0,
    ~0,
    BBAMI_PACKAGE_NAME                    , /* 80      Package-Name */
    BBAMI_ARCHIVE_ASSET_NAME              , /* 81      Archive-Asset-Name */
    ~0,
    ~0,
    BBAMI_PACKAGE_TYPE                    , /* 84      Package-Type */
    BBAMI_APPLICATION_CATEGORY            , /* 85      Application-Category */
    ~0,
    ~0,
    BBAMI_ARCHIVE_ASSET_SHA_512_DIGEST    , /* 88      Archive-Asset-SHA-512-Digest */
    ~0,
    ~0,
    ~0,
};

#define HASH_TABLE_SIZE (sizeof(attr_hash_table) / sizeof(attr_hash_table[0]))

static unsigned hash_str_impl(const char * str) {
    unsigned hash = strlen(str);
    unsigned c;
    while((c = *str++)!=0)
        hash = ((hash << 5) ^ (hash >> 27)) ^ c;
    return hash;
}

static unsigned hash_str(const char * str) {
    return hash_str_impl(str) % HASH_TABLE_SIZE;
}

static int hash_lookup(const char * str, bbami_attribute_id * attribute_id) {
    if(!str || !attribute_id)
        return EINVAL;
    if(!*str)
        return ENOENT;
    unsigned hash = hash_str(str);
    if (hash < HASH_TABLE_SIZE &&       /* hash value must not exceed table size */
        ~0 != attr_hash_table[hash] &&  /* hash value must map to the defined value */
        0 == strcmp(str, attr_names[attr_hash_table[hash]])) {
        *attribute_id = (bbami_attribute_id)attr_hash_table[hash];
        return EOK;
    }
    return ENOENT;
}

#endif /* __hash_table_definition_included__ */

