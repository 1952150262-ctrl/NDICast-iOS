"""从 macOS .pkg(xar) 提取 NDI SDK 的 iOS 库与头文件（支持 odc/newc cpio）"""
import gzip, os, struct, sys, tempfile, zlib, bz2
import xml.etree.ElementTree as ET

PKG = sys.argv[1] if len(sys.argv) > 1 else r"C:\Users\1\Desktop\Install_NDI_SDK_v6_Apple.pkg"
OUT = sys.argv[2] if len(sys.argv) > 2 else r"C:\Users\1\Desktop\ndi_sdk_extract"


def read_xar_toc(path):
    with open(path, "rb") as f:
        assert f.read(4) == b"xar!"
        _, _, toc_c, _, _ = struct.unpack(">HHQQI", f.read(24))
        toc = zlib.decompress(f.read(toc_c))
    return toc, 28 + toc_c


def payload_entries(toc):
    root = ET.fromstring(toc)
    out = []
    for el in root.iter("file"):
        name = el.findtext("name")
        data = el.find("data")
        if not name or data is None:
            continue
        out.append((name, int(data.findtext("offset")), int(data.findtext("length"))))
    return out


def decode_blob(raw):
    if raw[:2] == b"\x1f\x8b":
        return gzip.decompress(raw)
    if raw[:1] == b"\x78":
        return zlib.decompress(raw)
    if raw[:3] == b"BZh":
        return bz2.decompress(raw)
    return raw


def parse_cpio_stream(fh, handle):
    """自动识别 070707(odc) / 070701(newc)"""
    while True:
        magic = fh.read(6)
        if len(magic) < 6:
            return
        if magic == b"070707":
            rest = fh.read(70)          # dev,ino,mode,uid,gid,nlink,rdev(6*6) + mtime(11) + namesize(6) + filesize(11)
            def fld(s): 
                s = s.split(b"\0")[0].strip()
                return int(s, 8) if s else 0
            namesize = fld(rest[53:59])
            filesize = fld(rest[59:70])
            name = fh.read(namesize - 1).decode("utf-8", "replace")
            fh.read(1)
            data_pos = fh.tell()
            handle(name, filesize, data_pos, fh)
            fh.seek(data_pos + filesize)
            if name == "TRAILER!!!":
                return
        elif magic in (b"070701", b"070702"):
            rest = fh.read(104)
            def hx(i):
                s = rest[i * 8:(i + 1) * 8]
                return int(s, 16) if s else 0
            filesize = hx(6)
            namesize = hx(11)
            name = fh.read(namesize - 1).decode("utf-8", "replace")
            fh.read(1)
            pad = (4 - (110 + namesize) % 4) % 4
            fh.read(pad)
            data_pos = fh.tell()
            handle(name, filesize, data_pos, fh)
            fh.seek(data_pos + filesize + ((4 - filesize % 4) % 4))
            if name == "TRAILER!!!":
                return
        else:
            print("未知 cpio magic:", magic, "位置", fh.tell())
            return


def main():
    os.makedirs(OUT, exist_ok=True)
    toc, heap = read_xar_toc(PKG)
    entries = payload_entries(toc)
    print("xar 条目:", [e[0] for e in entries], flush=True)

    payload = [e for e in entries if e[0].lower() == "payload"][0]
    tmp = os.path.join(tempfile.gettempdir(), "ndi_payload.cpio")
    if os.path.exists(tmp) and os.path.getsize(tmp) > 100_000_000:
        print("复用已解出的 cpio", flush=True)
    else:
      with open(PKG, "rb") as f, open(tmp, "wb") as o:
        f.seek(heap + payload[1])
        raw = f.read(payload[2])
        o.write(decode_blob(raw))
    print("cpio 已解出:", tmp, os.path.getsize(tmp), "B", flush=True)

    kept = 0
    names = []

    def handle(name, size, pos, fh):
        nonlocal kept
        low = name.lower()
        if ("ndi" in low and (low.endswith(".a") or low.endswith(".dylib"))) or low.endswith(".h"):
            names.append((name, size))
            cur = fh.tell()
            fh.seek(pos)
            data = fh.read(size)
            dst = os.path.join(OUT, name.lstrip("./").replace("../", "_").replace("/", os.sep))
            os.makedirs(os.path.dirname(dst) or OUT, exist_ok=True)
            with open(dst, "wb") as w:
                w.write(data)
            fh.seek(cur)
            kept += 1

    with open(tmp, "rb") as fh:
        parse_cpio_stream(fh, handle)

    print("提取文件数:", kept)
    for n, s in names[:60]:
        print(f"  {s:>12,} B  {n}")


if __name__ == "__main__":
    main()
