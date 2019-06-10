import unittest
import json
import subprocess


class TestBufr2Json(unittest.TestCase):
    def test_bufr2json(self):
        geojson = json.loads(subprocess.check_output(["./bufr2json", "test.bufr"]))
        self.assertEqual(len(geojson.keys()), 2)
        self.assertEqual(geojson["type"], "FeatureCollection")
        self.assertEqual(len(geojson["features"]), 2)
        feature = geojson["features"][0]
        self.assertEqual(len(feature.keys()), 3)
        self.assertEqual(feature["type"], "Feature")
        self.assertEqual(feature["geometry"]["type"], "Point")
        self.assertEqual(feature["geometry"]["coordinates"], [44.60016, 12.07738])
        properties = feature["properties"]
        self.assertEqual(properties["lon"], 4460016)
        self.assertEqual(properties["lat"], 1207738)
        self.assertEqual(properties["datetime"], "2019-01-02T03:04:00Z")
        self.assertEqual(properties["network"], "locali")
        self.assertEqual(properties["ident"], None)
        self.assertEqual(properties["level_t1"], 103)
        self.assertEqual(properties["level_v1"], 2000)
        self.assertEqual(properties["level_t2"], None)
        self.assertEqual(properties["level_v2"], None)
        self.assertEqual(properties["trange_pind"], 254)
        self.assertEqual(properties["trange_p1"], 0)
        self.assertEqual(properties["trange_p2"], 0)
        self.assertEqual(properties["bcode"], "B12101")
        self.assertEqual(properties["value"], 285.45)


if __name__ == '__main__':
    unittest.main()
