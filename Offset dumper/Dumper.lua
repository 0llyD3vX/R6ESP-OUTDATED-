-- =============================================
-- R6S ViewData Finder (External Camera Resolver)
-- Scans all committed memory for the ViewData constant float pattern
--
-- ViewData offsets:
--   +0x190 = Camera Position (Vector3)
--   +0x250 = View-Projection Matrix (4x4 float)
--   +0x2A4 = Constant float pattern (scan target)
-- =============================================
 
print("=== R6S ViewData Finder ===")
print("")
 
local PATTERN = "A4 70 7D BF 00 00 00 00 00 00 00 00 00 00 A0 40 00 00 A0 C0 00 00 00 00 00 00 00 00 CD CC 4C 3F 00 00 00 3F 00 00 80 3E"
local VD_OFFSET = -0x2A4
 
print("[*] Pattern: " .. PATTERN)
print("[*] Scanning ALL committed memory...")
print("")
 
local scan = createMemScan()
scan.OnlyOneResult = false
scan.firstScan(soExactValue, vtByteArray, nil, PATTERN, "",
              0, 0x7FFFFFFFFFFF, "", fsmNotAligned, "1", true, true, false, false)
scan.waitTillDone()
 
local foundlist = createFoundList(scan)
foundlist.initialize()
local count = foundlist.Count
 
print("[*] Found " .. count .. " matches")
print("")
 
if count == 0 then
    print("[!] ViewData pattern NOT FOUND.")
    print("[!] Make sure you're in a match or firing range.")
else
    for i = 0, count - 1 do
        local addr = tonumber("0x" .. foundlist.Address[i])
        if addr then
            local viewdata = addr + VD_OFFSET
 
            print(string.format("=== HIT %d ===", i))
            print(string.format("  Pattern Address : 0x%X", addr))
            print(string.format("  ViewData Base   : 0x%X", viewdata))
            print("")
 
            local cx = readFloat(viewdata + 0x190) or 0
            local cy = readFloat(viewdata + 0x194) or 0
            local cz = readFloat(viewdata + 0x198) or 0
            print(string.format("  Camera Position : %.4f, %.4f, %.4f", cx, cy, cz))
            print("")
 
            print("  View-Projection Matrix (ViewData + 0x250):")
            for row = 0, 3 do
                local m0 = readFloat(viewdata + 0x250 + row * 16 + 0) or 0
                local m1 = readFloat(viewdata + 0x250 + row * 16 + 4) or 0
                local m2 = readFloat(viewdata + 0x250 + row * 16 + 8) or 0
                local m3 = readFloat(viewdata + 0x250 + row * 16 + 12) or 0
                print(string.format("    [%d] %10.6f %10.6f %10.6f %10.6f", row, m0, m1, m2, m3))
            end
            print("")
 
            local testX, testY, testZ = 100.0, 100.0, 100.0
            local m = {}
            for j = 0, 15 do
                m[j] = readFloat(viewdata + 0x250 + j * 4) or 0
            end
 
            local clipX = testX * m[0] + testY * m[4] + testZ * m[8]  + m[12]
            local clipY = testX * m[1] + testY * m[5] + testZ * m[9]  + m[13]
            local clipW = testX * m[3] + testY * m[7] + testZ * m[11] + m[15]
 
            print(string.format("  W2S Test (100,100,100): clipX=%.2f clipY=%.2f clipW=%.2f", clipX, clipY, clipW))
 
            if clipW > 0.001 then
                local sx = 960 + (clipX / clipW) * 960
                local sy = 540 - (clipY / clipW) * 540
                print(string.format("  Screen Coords: %.1f, %.1f", sx, sy))
                print(string.format("  On Screen: %s", (sx > 0 and sx < 1920 and sy > 0 and sy < 1080) and "YES" or "NO"))
            else
                print("  Behind camera (clipW <= 0)")
            end
            print("")
        end
    end
end
 
foundlist.destroy()
scan.destroy()
 
print("=== Scan Complete ===")
print("")
print("USAGE IN YOUR ESP:")
print("  ViewMatrix = read<Matrix4x4>(ViewData + 0x250)")
print("  CameraPos  = read<Vector3>(ViewData + 0x190)")