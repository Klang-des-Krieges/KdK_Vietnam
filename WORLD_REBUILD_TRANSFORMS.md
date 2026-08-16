# KdK Vietnam – Transform reference for fresh world rebuild

Source snapshot: existing `world/KdK_Vietnam_Layers` before the fresh-world rebuild.

## Counts

- Neutral Conflict military bases: **25**
- Standalone FIA ambient patrol spawnpoints: **223**
- FIA patrol spawnpoints with explicit angles: **0**
- Therefore all standalone FIA patrol spawnpoints use the prefab/default orientation: **0 0 0**

The two faction HQ bases are deliberately excluded from the neutral-base count.
Ambient patrol components embedded as children of military bases are deliberately excluded from the standalone FIA patrol count.

## Neutral military bases

Format: `Layer/index | coords X Y Z | angles X Y Z`

```text
Airstrip/1          | 2728.197 29.906 5228.381  | 0 73.687 0
CaveVillage/1       | 4463.293 52.537 820.870    | 0 41.503 0
Checkpoint2/1       | 4789.236 36.663 1706.196   | 0 168.789 0
City2/1             | 3465.918 32.562 5659.112   | 0 147.461 0
City2/2             | 3760.654 32.562 5775.848   | 0 56.522 0
DeepJungle1/1       | 7498.026 218.729 284.286   | 0.474 70.230 -3.969
DeepJungle2/1       | 7371.583 142.646 1227.867  | -6.071 13.487 -1.453
DeepJungleNorth1/1  | 833.176 30.691 6374.613    | 0.229 104.804 -0.865
DeepJungleNorth1/2  | 2543.656 50.688 6974.891   | -0.869 -13.758 0.213
DeepJungleNorth1/3  | 4395.573 21.153 5350.126   | -2.095 73.980 -4.046
Farmhouse1/1        | 5645.162 42.891 2427.020   | -0.250 73.763 -0.859
Islands/1           | 7456.752 35.002 5455.119   | 0.810 -154.840 0.380
Islands/2           | 6802.600 37.303 7292.435   | -0.462 -58.858 0.766
Islands/3           | 5275.538 38.015 5590.893   | -0.250 73.763 -0.859
LakeVillage1/1      | 2147.986 28.279 5212.202   | 0 -149.728 0
NoMansLand/1        | 1742.895 61.142 3016.348   | -4.775 -26.381 -5.584
NoMansLand/2        | 2130.498 108.920 2679.912  | 0 -41.500 0
PleasantValley/1    | 2120.990 64.375 603.086    | 0 173.302 0
PleasantValley/2    | 3359.177 167.176 297.331   | -2.704 -35.511 0.830
RoadsideBase/1      | 1319.576 49.092 1612.327   | 0 -70.431 0
RockCrags/1         | 2904.623 30.804 4115.906   | 0 -149.728 0
RockCrags/2         | 1004.350 43.669 1074.401   | 0 -162.139 0
SwampVillage/1      | 1148.576 23.623 4970.293   | 0 80.845 0
TempleRuins/1       | 3018.611 297.369 1814.434  | 0 57.694 0
USSR_Valley/1       | 2841.155 44.945 6090.222   | 0 73.687 0
```

## FIA ambient patrol spawnpoints

Authoritative transform source:

`world/KdK_Vietnam_Layers/KdK_Edits/Managers.layer`

The standalone group begins with:

`$grp GenericEntity : "{9273AB931008C271}Prefabs/Systems/AmbientPatrol/AmbientPatrolSpawnpoint_FIA.et"`

It contains exactly **223** instances. Every instance has a world-space `coords` value and no explicit `angles` value. During the later transfer, copy the 223 `coords` entries in their existing order and retain the new entities' default orientation `0 0 0`.

## Later transfer procedure

1. In the fresh world, place exactly 25 correct neutral-base prefabs.
2. Place exactly 223 correct standalone FIA ambient-patrol prefabs.
3. Save the fresh world.
4. Replace only the fresh entities' `coords` and `angles` values using this snapshot and the authoritative old `Managers.layer`.
5. Do not copy components, faction affiliations, child entities, inherited overrides, prefab references, or IDs from the old world.
