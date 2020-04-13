begin;
    select plan (24);
    
    --
    -- check user_id column properties
    --
    select has_column ('tb_user', 'user_id');
    select col_is_pk ('tb_user', 'user_id');

    --
    -- check user_firstname column properties
    --
    select has_column ('tb_user', 'user_firstname');
    select col_type_is ('tb_user', 'user_firstname', 'character varying(31)');
    select col_not_null ('tb_user', 'user_firstname');


    --
    -- check user_familyname column properties
    --
    select has_column ('tb_user', 'user_familyname');
    select has_index ('tb_user', 'ix_user_familyname', 'user_familyname');
    select col_type_is ('tb_user', 'user_familyname', 'character varying(62)');
    select col_not_null ('tb_user', 'user_familyname');
    
    --
    -- check user_email column properties
    --
    select has_column ('tb_user', 'user_email');
    select has_index ('tb_user', 'ix_user_email', 'user_email');
    select col_type_is ('tb_user', 'user_email', 'character varying(71)');
    select col_not_null ('tb_user', 'user_email');
    
    --
    -- check user_telephone column properties
    --
    select has_column ('tb_user', 'user_telephone');
    select col_type_is ('tb_user', 'user_telephone', 'character varying(23)');
    select col_not_null ('tb_user', 'user_telephone');

    --
    -- check user_gender column properties
    --
    select has_column ('tb_user', 'user_gender');
    select col_type_is ('tb_user', 'user_gender', 'character(1)');
    select col_not_null ('tb_user', 'user_gender');

    --
    -- check user_crtime column properties
    --
    select has_column ('tb_user', 'user_crtime');
    select has_index ('tb_user', 'ix_user_crtime', 'user_crtime');
    select col_type_is ('tb_user', 'user_crtime', 'timestamp without time zone');
    select col_has_default ('tb_user', 'user_crtime');
    select col_default_is ('tb_user', 'user_crtime', 'CURRENT_TIMESTAMP');

    select * from finish ();
rollback;

