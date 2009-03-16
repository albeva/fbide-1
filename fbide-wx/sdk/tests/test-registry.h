	/**
	 * Check registry and variants
	 */

	CRegistry r;
    wxString        key1 = _T("key1");
    const char    * key2 = "key2";
    const wxChar  * key3 = _T("key3");

    // test against "const wxChar *"
    r[key1] = _T("Hello");
    r[key1] += _T(" world");
    wxASSERT(r[key1] == _T("Hello world"));
    r[key1] += _T("!");
    wxASSERT(r[key1] != _T("Hello world"));

    // test against CVariant
    r[key2] = r[key1];
    wxASSERT(r[key2] == r[key1]);
    r[key2] += r[key1];
    wxASSERT(r[key2] != r[key1]);

    // test against const char *
    r[key3] = "const";
    r[key3] +=  " char";
    wxASSERT(r[key3] == "const char");
    r[key3] +=  " *";
    wxASSERT(r[key3] != _T("const char"));

    // test against wxString
    r["key4"] = wxString(_T("wx"));
    r["key4"] +=  wxString(_T("String"));
    wxASSERT(r["key4"] == wxString(_T("wxString")));
    r["key4"] +=  wxString(_T("()"));
    wxASSERT(r["key4"] != wxString(_T("wxString")));

    // test against int
    r["int1"] = 10;
    r["int2"] = r["int1"].AsInt();
    wxASSERT(r["int2"] == 10);
    wxASSERT(r["int1"] != 50);
    wxASSERT(r["int1"] == "10");
    r["int3"] = -15;
    wxASSERT(r["int3"] == "-15");
    wxASSERT(r["int3"] == -15);
    r["int3"] += 20;
    wxASSERT(r["int3"] == 5);

    // test againnst bool
    r["bool1"] = true;
    r["bool2"] = false;
    wxASSERT(r["bool1"] == true);
    wxASSERT(r["bool1"] == "true");
    wxASSERT(r["bool1"] == 1);
    wxASSERT(r["int3"] == true); // test against another numic value
    wxASSERT(r["bool2"] != true);
    r["bool4"] = (int)true;
    wxASSERT(r["bool4"] == true);
    wxASSERT(r["bool4"] != false);
    wxASSERT(r["bool4"] == 1);
    wxASSERT(r["bool4"] != 0);
    wxASSERT(r["bool4"] == "1");
    wxASSERT(r["bool4"] != "2");
    wxASSERT(r["bool4"].AsInt() == (int)true);
    r["bool5"] = false;
    wxASSERT(r["bool5"] == false);
    wxASSERT(r["bool5"] != true);
    wxASSERT(r["bool5"] == "false");
    wxASSERT(r["bool5"] != "true");
    wxASSERT(r["bool5"] == 0);
    r["bool6"] = (int)false;
    wxASSERT(r["bool6"] == false);
    wxASSERT(r["bool6"] != true);
    wxASSERT(r["bool6"] == "0");
    wxASSERT(r["bool6"] != "1");
    wxASSERT(r["bool6"] == 0);
    wxASSERT(r["bool6"] != 1);