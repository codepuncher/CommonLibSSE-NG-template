# Releasing

## Nexus Mods upload

`nexus-upload.yml` runs on a `workflow_run` trigger chained off `release.yml`: `release.yml` publishes the GitHub Release using the default `GITHUB_TOKEN`, and GitHub doesn't run a `release: published`-triggered workflow off events caused by `GITHUB_TOKEN`, so `workflow_run` is used instead (it fires on the completion of `release.yml` itself, regardless of what token published inside it). The `upload` job runs under the `nexus` [GitHub environment](https://docs.github.com/en/actions/deployment/targeting-different-environments/using-environments-for-deployment), which requires manual approval before the job proceeds and scopes the Nexus secrets to that environment. It can also be triggered manually via **workflow_dispatch** with the version (no `v` prefix), e.g. to re-run a failed upload; this still requires the same environment approval.

Because `workflow_run` always runs the copy of `nexus-upload.yml` committed to `main` (not whatever's on a feature branch), changes to this workflow only take effect after merging to `main`.

Do not restrict the `nexus` environment to tags: `workflow_run`-triggered jobs always execute against the default branch's ref (`refs/heads/main`), not the tag that triggered the upstream `release.yml` run, so a tag-only policy would silently block every auto-triggered upload. A deployment-branch policy limited to `main` is compatible with the auto-trigger and keeps `workflow_dispatch` runs from other branches from reaching the secrets.

**Prerequisites (one-time setup):**

1. Upload your first file manually via the [Nexus Mods web UI](https://www.nexusmods.com): this creates the file that later uploads add versions to.
2. Note its file ID from the **API Info** option on the mod page's Files tab, or from the file's edit menu on the Manage Files page.
3. Create the `nexus` environment (Settings → Environments → New environment) with a required reviewer, before the workflow referencing it is merged to `main`. Otherwise GitHub auto-creates it unprotected on first reference.
4. Add to the `nexus` environment as secrets (Settings → Environments → `nexus` → Environment secrets):
   - `NEXUSMODS_API_KEY`: your Nexus Mods API key
   - `NEXUSMODS_FILE_ID`: the file ID
   - `NEXUSMODS_MOD_ID`: the mod's internal ID, used to post each release's notes to the mod's Changelog tab. **Not** the number in the mod page URL. Look that URL number up via `https://api.nexusmods.com/v3/games/skyrimspecialedition/mods/<url-id>` (needs an `apikey` header) and use the `id` field from the response.
5. Add to the `nexus` environment as a variable (Settings → Environments → `nexus` → Environment variables):
   - `NEXUSMODS_DISPLAY_NAME`: the file name shown on Nexus
