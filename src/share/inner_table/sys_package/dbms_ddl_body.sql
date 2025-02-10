#package_name: DBMS_DDL
#author: haohao.hao

CREATE OR REPLACE PACKAGE BODY DBMS_DDL AS
  FUNCTION INTERNAL_WRAP(DDL DBMS_SQL.VARCHAR2S, LB PLS_INTEGER, UB PLS_INTEGER) RETURN CLOB;
    PRAGMA INTERFACE(C, DBMS_DDL_WRAP_VS);
  FUNCTION INTERNAL_WRAP(DDL DBMS_SQL.VARCHAR2A, LB PLS_INTEGER, UB PLS_INTEGER) RETURN CLOB;
    PRAGMA INTERFACE(C, DBMS_DDL_WRAP_VA);

  FUNCTION WRAP(DDL VARCHAR2) RETURN VARCHAR2;
    PRAGMA INTERFACE(C, DBMS_DDL_WRAP);

  FUNCTION WRAP(DDL DBMS_SQL.VARCHAR2S, LB PLS_INTEGER, UB PLS_INTEGER) RETURN DBMS_SQL.VARCHAR2S
  IS
    WRAPPED      CLOB;
    REMAINS      PLS_INTEGER;
    RET_ARR      DBMS_SQL.VARCHAR2S;
    VARCHAR_LEN  PLS_INTEGER := 256;
  BEGIN
    WRAPPED := INTERNAL_WRAP(DDL, LB, UB);
    REMAINS := DBMS_LOB.GETLENGTH(WRAPPED);
    WHILE REMAINS > 0 LOOP
      DBMS_LOB.READ(WRAPPED, VARCHAR_LEN, RET_ARR.COUNT * VARCHAR_LEN + 1, RET_ARR(RET_ARR.COUNT + 1));
      REMAINS := REMAINS - VARCHAR_LEN;
    END LOOP;
    RETURN RET_ARR;
  END;

  FUNCTION WRAP(DDL DBMS_SQL.VARCHAR2A, LB PLS_INTEGER, UB PLS_INTEGER) RETURN DBMS_SQL.VARCHAR2A
  IS
    WRAPPED      CLOB;
    REMAINS      PLS_INTEGER;
    RET_ARR      DBMS_SQL.VARCHAR2A;
    VARCHAR_LEN  PLS_INTEGER := 32767;
  BEGIN
    WRAPPED := INTERNAL_WRAP(DDL, LB, UB);
    REMAINS := DBMS_LOB.GETLENGTH(WRAPPED);
    WHILE REMAINS > 0 LOOP
      DBMS_LOB.READ(WRAPPED, VARCHAR_LEN, RET_ARR.COUNT * VARCHAR_LEN + 1, RET_ARR(RET_ARR.COUNT + 1));
      REMAINS := REMAINS - VARCHAR_LEN;
    END LOOP;
    RETURN RET_ARR;
  END;

  PROCEDURE CREATE_WRAPPED(DDL VARCHAR2);
    PRAGMA INTERFACE(C, DBMS_DDL_CREATE_WRAPPED);
  PROCEDURE CREATE_WRAPPED(DDL DBMS_SQL.VARCHAR2S, LB PLS_INTEGER, UB PLS_INTEGER);
    PRAGMA INTERFACE(C, DBMS_DDL_CREATE_WRAPPED_VS);
  PROCEDURE CREATE_WRAPPED(DDL DBMS_SQL.VARCHAR2A, LB PLS_INTEGER, UB PLS_INTEGER);
    PRAGMA INTERFACE(C, DBMS_DDL_CREATE_WRAPPED_VA);
END DBMS_DDL;
//
