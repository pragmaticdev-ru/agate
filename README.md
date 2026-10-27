# Agate

**Peer-to-peer synchronization and collaboration on engineering
documentation.** Connect a project folder on one computer, invite your team —
and every drawing, sheet and spec stays in sync directly between your
machines. No server, no cloud, no account registrar: your files travel
device-to-device, end-to-end encrypted.

Agate is a native desktop application for Windows and Linux built for the
daily grind of engineering review: thousand-sheet geodesy drawings, revision
comparison, remarks with discussions, voice briefings right on the drawing.

![Canvas](docs/screenshots/12-canvas-overview.png)

## Getting started

Download the portable binary for your OS from the
[latest release](https://github.com/pragmaticdev-ru/agate/releases/latest) —
no installer, no runtime dependencies:

| Asset | Platform |
|---|---|
| `agate.exe` | Windows 10+ x64 |
| `agate` | Linux x64 (X11 or Wayland; ALSA/PipeWire for voice) |

Run it, sign in with your e-mail — a profile with its device keys is created
on the spot and never leaves the machine.

![Sign in](docs/screenshots/01-login.png)

## Folders that sync themselves

Connect any local folder and it becomes a synchronized **area**: Agate
watches it, versions every change and exchanges content with the other
participants. Two views cover two jobs: **My folders** for your synchronized
areas, **All folders** for the whole drive — with the familiar breadcrumb,
tree and type-ahead of a file manager.

| | |
|---|---|
| ![My folders](docs/screenshots/02-explorer-my-folders.png) | ![All folders](docs/screenshots/03-explorer-all-folders.png) |

The explorer is a real file manager: icon, list, table and tiles views,
sortable columns (name, date, author, type, size), thumbnails for documents,
and a per-file **sync** column that tells you exactly what is happening —
*synchronized*, *publishing*, *downloading 42 %*, or the honest reason it
cannot proceed.

| | |
|---|---|
| ![Icons view](docs/screenshots/04-explorer-icons.png) | ![Table view](docs/screenshots/05-explorer-table.png) |

Every file operation is at hand in the context menu — open on canvas, version
history, access control, reveal on disk:

![Context menu](docs/screenshots/06-context-menu.png)

## Access control without a server

Invitations are the only way in. A manager of a folder hands over an **invite
link** and a **PIN** — deliberately over two different channels (link by
e-mail, PIN by phone): the link finds the folder, the PIN enters it. Roles
are explicit (viewer, editor, manager), every participant and pending
invitation is visible, and revoking hides the whole subtree from that person.

| | |
|---|---|
| ![Access participants](docs/screenshots/07-access-participants.png) | ![Invite link and PIN](docs/screenshots/08-access-invite.png) |

## Versions, kept honestly

Every committed change is a version — with its author, timestamp and stored
size. Nothing is silently overwritten, any version can be restored or
compared, and pinned versions survive cache eviction.

![Version history](docs/screenshots/09-version-history.png)

## A canvas built for heavy drawings

PDF drawings render through a GPU compute pipeline (vello/wgpu) that keeps
multi-hundred-megabyte sheets fluid: tiled chunks, on-demand LOD, and a
guarded memory budget. Eight-megabyte geodesy bases with dense linework open
and pan at full frame rate; deep zoom reveals the actual vector geometry, not
a blurry raster.

| | |
|---|---|
| ![Deep zoom 1](docs/screenshots/13-canvas-dense-1.png) | ![Deep zoom 2](docs/screenshots/14-canvas-dense-2.png) |
| ![Deep zoom 3](docs/screenshots/15-canvas-dense-3.png) | ![100% zoom](docs/screenshots/16-canvas-100.png) |

Multi-sheet documents are first-class: page turning, a controlled page field,
sheet-by-sheet navigation and per-page state.

## Remarks with discussions

Mark up the drawing with the pen, shapes, pins, text chips and arrow tools;
each markup set becomes a **remark** with an author, a status (*open*,
*question*, *resolved*, *fix*) and a comment thread. The remarks panel lists
every remark of the document with unread tracking; clicking a mark on the
canvas opens a compact peek card with the discussion.

| | |
|---|---|
| ![Draft composer](docs/screenshots/17-draft-composer.png) | ![Remarks panel](docs/screenshots/18-remarks-panel.png) |
| ![Remark peek](docs/screenshots/19-remark-peek.png) | ![Tool panel](docs/screenshots/20-canvas-panel.png) |

Measurements belong to the drawing: length and area tools read true drawing
units (mm, cm, m²) off the PDF geometry and stay attached to the sheet.

![Measurement](docs/screenshots/21-measure-length.png)

## Version comparison

Pick any two versions and the canvas shows what actually changed: additions
in green, removals in red, with unchanged geometry faded out — so a sheet
that carries a thousand lines can still be reviewed in seconds.

Two revisions of the same drawing are almost never an exact copy of each
other: a re-export or a scan shifts the sheet a little and stretches it a
bit. Before highlighting anything, Agate **aligns the two revisions by their
content** — it finds matching parts of the drawing and snaps the sheets onto
each other, compensating for that shift and stretch. The highlighting then
lands exactly on the changed elements instead of smearing across the whole
sheet, and lines that did not change stay exactly on top of each other.

The alignment holds at any zoom. On the overview you see where the changes
cluster; zoom in, and the highlighted areas resolve into the actual edited
lines — still sharp, still in place, because the two sheets remain locked
together pixel-tight.

| | |
|---|---|
| ![Comparison overview](docs/screenshots/22-compare-overview.png) | ![Comparison zoomed](docs/screenshots/23-compare-zoom.png) |
| ![Removals](docs/screenshots/24-compare-removals.png) | ![Live comparison](docs/screenshots/25-compare-live.png) |

Marks anchored to older versions stay visible in grey exactly where they
were made, so nothing from the discussion gets lost in a revision.

## Together on one drawing

Open the same document and the participants panel shows who is here: names,
connection state, and the live meeting timer. One click on the meeting pill
puts you on air — the panel carries the roster with the «On air» line, the
microphone and the leave control, and the pill itself keeps the participant
counter while the panel is closed.

| | |
|---|---|
| ![On air, full view](docs/screenshots/26-presence.png) | ![Participants panel](docs/screenshots/27-presence-panel.png) |
| ![Microphone off](docs/screenshots/28-voice-muted.png) | ![Meeting controls after the meeting](docs/screenshots/29-voice-pill-badge.png) |

A remark made during the meeting remembers the moment it was made. Open such
a remark's thread in the panel and it offers **Listen to the context** — the
recording of that very meeting, with playback starting a few seconds *before*
the remark, so you hear the discussion that led to it. The player shows the
meeting's recordings with play/pause, a seek bar, the position and the
volume.

| | |
|---|---|
| ![Listen to the context](docs/screenshots/30-remark-listen.png) | ![Recording player](docs/screenshots/31-recording-player.png) |

Every meeting is recorded losslessly, so latecomers catch up from the
recordings list — and remarks keep their link into the exact fragment that
covers them.

## Settings that tell the truth

Storage directories and limits (with a separate, never-evicted budget for
meeting recordings), the network port and the real state of inbound firewall
access, the signed self-update channel, and the interface language — English
and Russian, applied on the next start.

| | |
|---|---|
| ![Settings](docs/screenshots/10-settings.png) | ![Language](docs/screenshots/11-settings-language.png) |

## How synchronization works

- **Pure pull anti-entropy.** Nodes exchange lossy hints (gossip) and pull
  the actual content — manifests and content-addressed chunks — from the
  holders they trust. There is no push data path and no server in the middle.
- **Content-addressed storage.** Files are cut into chunks, addressed by
  their hash, deduplicated across versions and shared by reference; the
  local cache is bounded and pinned versions are never evicted.
- **Offline-first.** Everything works with the network down: local edits
  commit and version locally, then replicate when a holder reappears.
- **Converter plugins.** Unsupported formats (DWG, DXF and friends) convert
  through out-of-process plugin converters with a cached, bounded conversion
  cache.

## Trust and safety

- End-to-end encrypted transports (Noise, libp2p); keys are created on the
  device and never leave it.
- A device key is only as good as its proof: direct human confirmation,
  SAS pairing, or a signed credential chain from an already-trusted key —
  TOFU alone unlocks nothing.
- Updates come from an Ed25519-signed channel; the binary verifies the
  signature itself before installing anything.

## Under the hood

A single Rust binary with no runtime DLLs: `iced`/wgpu UI with a vendored
compute-`vello` renderer, statically linked pdfium for PDF parsing, libp2p
for transport, SQLite for state, Opus/sonora for meeting audio. The same
executable is the out-of-process host for converter plugins and the
elevated firewall helper.

## License

BSD-3-Clause.
