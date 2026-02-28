# Repository split

Per request, the Uni Heidelberg Flipper app is now isolated under:

- `unihd_flipper_app/`

This folder is a standalone `ufbt` external app project (`application.fam` included).

## Create it as a separate git repository

Run:

```bash
./tools/export_unihd_repo.sh ../unihd-card-flipper-app
```

This creates a new independent git repo at `../unihd-card-flipper-app` with only the Flipper app contents.
