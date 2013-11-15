#include <string.h>

static vector<string> g_includePaths;

void
cset_include_paths(const vector<string>& includePaths)
{
    g_includePaths = includePaths;
}

// ==========================================================
int
ccompile_aidl(Options& options)
{
    int err = 0, N;

    cset_include_paths(options.importPaths);

    cregister_base_types();

    // import the preprocessed file
    N = options.preprocessedFiles.size();
    for (int i=0; i<N; i++) {
        const string& s = options.preprocessedFiles[i];
        err |= parse_preprocessed_file(s);
    }
    if (err != 0) {
        return err;
    }

    // parse the main file
    g_callbacks = &g_mainCallbacks;
    err = parse_aidl(options.inputFileName.c_str());
    document_item_type* mainDoc = g_document;
    g_document = NULL;

    // parse the imports
    g_callbacks = &g_mainCallbacks;
    import_info* import = g_imports;
    while (import) {
        if (NAMES.Find(import->neededClass) == NULL) {
            import->filename = find_import_file(import->neededClass);
            if (!import->filename) {
                fprintf(stderr, "%s:%d: couldn't find import for class %s\n",
                        import->from, import->statement.lineno,
                        import->neededClass);
                err |= 1;
            } else {
                err |= parse_aidl(import->filename);
                import->doc = g_document;
                if (import->doc == NULL) {
                    err |= 1;
                }
            }
        }
        import = import->next;
    }
    // bail out now if parsing wasn't successful
    if (err != 0 || mainDoc == NULL) {
        //fprintf(stderr, "aidl: parsing failed, stopping.\n");
        return 1;
    }

    // complain about ones that aren't in the right files
    err |= check_filenames(options.inputFileName.c_str(), mainDoc);
    import = g_imports;
    while (import) {
        err |= check_filenames(import->filename, import->doc);
        import = import->next;
    }

    // gather the types that have been declared
    err |= gather_types(options.inputFileName.c_str(), mainDoc);
    import = g_imports;
    while (import) {
        err |= gather_types(import->filename, import->doc);
        import = import->next;
    }

#if 0
    printf("---- main doc ----\n");
    test_document(mainDoc);

    import = g_imports;
    while (import) {
        printf("---- import doc ----\n");
        test_document(import->doc);
        import = import->next;
    }
    NAMES.Dump();
#endif

    // check the referenced types in mainDoc to make sure we've imported them
    err |= check_types(options.inputFileName.c_str(), mainDoc);

    // finally, there really only needs to be one thing in mainDoc, and it
    // needs to be an interface.
    bool onlyParcelable = false;
    err |= exactly_one_interface(options.inputFileName.c_str(), mainDoc, options, &onlyParcelable);

    // If this includes an interface definition, then assign method ids and validate.
    if (!onlyParcelable) {
        err |= check_and_assign_method_ids(options.inputFileName.c_str(),
                ((interface_type*)mainDoc)->interface_items);
    }

    // after this, there shouldn't be any more errors because of the
    // input.
    if (err != 0 || mainDoc == NULL) {
        return 1;
    }

    // if needed, generate the outputFileName from the outputBaseFolder
    if (options.outputFileName.length() == 0 &&
            options.outputBaseFolder.length() > 0) {
        options.outputFileName = generate_outputFileName(options, mainDoc);
    }

    // if we were asked to, generate a make dependency file
    // unless it's a parcelable *and* it's supposed to fail on parcelable
    if ((options.autoDepFile || options.depFileName != "") &&
            !(onlyParcelable && options.failOnParcelable)) {
        // make sure the folders of the output file all exists
        check_outputFilePath(options.outputFileName);
        generate_dep_file(options, mainDoc);
    }

    // they didn't ask to fail on parcelables, so just exit quietly.
    if (onlyParcelable && !options.failOnParcelable) {
        return 0;
    }

    // make sure the folders of the output file all exists
    check_outputFilePath(options.outputFileName);

    err = generate_java(options.outputFileName, options.inputFileName.c_str(),
                        (interface_type*)mainDoc);

    return err;
}
