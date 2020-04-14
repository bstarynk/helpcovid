begin;
    select plan(12);

    --
    -- check function signature
    --
    select has_function('fn_user_status', ARRAY['character varying']);
    select function_returns('fn_user_status', 'integer');
    select function_lang_is('fn_user_status', 'plpgsql');

    --
    -- check call to fn_user_status('INACTIVE')
    --
    select lives_ok($$ select fn_user_status('INACTIVE'); $$);
    select results_eq($$ select fn_user_status('INACTIVE'); $$, $$ select 0 $$);

    --
    -- check call to fn_user_status('ACTIVE')
    --
    select lives_ok($$ select fn_user_status('ACTIVE'); $$);
    select results_eq($$ select fn_user_status('ACTIVE'); $$, $$ select 1 $$);
    
    --
    -- check call to fn_user_status('__MIN__')
    --
    select lives_ok($$ select fn_user_status ('__MIN__'); $$);
    select results_eq($$ select fn_user_status ('__MIN__'); $$, $$ select 0 $$);

    --
    -- check call to fn_user_status('__MAX__')
    --
    select lives_ok($$ select fn_user_status('__MAX__'); $$);
    select results_eq($$ select fn_user_status('__MAX__'); $$, $$ select 1 $$);

    --
    -- check call to invalid user_status ('FOO')
    --
    select throws_ok($$ select fn_user_status ('FOO'); $$);

    select * from finish();
rollback;

