# NERVA Ring — Website

A self-contained marketing/landing site for the NERVA Ring, an open-source
cognitive-health smart ring.

## Files
- `index.html` — the page (semantic HTML, no build step)
- `styles.css` — all styling (dark theme, responsive, CSS/SVG graphics only — no external assets)

## Running locally
It's a static site, so just open the file or serve the folder:

```bash
cd website
python3 -m http.server 8080
# then visit http://localhost:8080
```

## Deploying with GitHub Pages
Point Pages at this folder, or copy its contents to your Pages source
(e.g. `/docs` or the `gh-pages` branch). Everything is inlined/self-contained,
so no bundler or CDN is required.

Content is sourced from the repository README (hardware, sensors, BLE UUIDs)
and the app/firmware docs (Cognitive Score, stress patterns, activity
recognition).
