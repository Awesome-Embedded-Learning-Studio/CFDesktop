#!/usr/bin/env python3
"""
MkDocs 404 Link Scanner
========================
Scans the MkDocs development server for broken internal links.

Usage:
    .venv/bin/python scripts/document/check_404_links.py           # HandBook only
    .venv/bin/python scripts/document/check_404_links.py --all      # All pages
    .venv/bin/python scripts/document/check_404_links.py --api-only # API pages only

Strategy:
    Phase 1 - Collect all internal links from every page (concurrent)
    Phase 2 - Check each *unique* resolved URL exactly once (no duplicates)
    Phase 3 - Map broken URLs back to their source pages
"""

import re
import sys
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor, as_completed
from html.parser import HTMLParser
from urllib.parse import urljoin

import requests

BASE_URL = "http://127.0.0.1:8000/CFDesktop/"
MAX_WORKERS = 8


class LinkExtractor(HTMLParser):
    def __init__(self):
        super().__init__()
        self.links = []

    def handle_starttag(self, tag, attrs):
        if tag == "a":
            for attr, value in attrs:
                if attr == "href" and value:
                    self.links.append(value)


def is_internal(href):
    if not href:
        return False
    if href.startswith(("#", "javascript:", "mailto:", "tel:")):
        return False
    if href.startswith("http") and BASE_URL.rstrip("/") not in href:
        return False
    if re.search(r"\.(css|js|png|jpg|jpeg|gif|svg|ico|woff2?|ttf|eot)(\?|$)", href):
        return False
    return True


def fetch(url, timeout=10):
    try:
        resp = requests.get(url, timeout=timeout)
        return resp.status_code, resp.text
    except requests.RequestException:
        return None, ""


def collect_links(page_url):
    """Fetch a page and return (page_url, [resolved_links])."""
    status, html = fetch(page_url)
    if status != 200:
        return page_url, []
    parser = LinkExtractor()
    try:
        parser.feed(html)
    except Exception:
        return page_url, []

    resolved = set()
    for href in parser.links:
        if not is_internal(href):
            continue
        resolved.add(urljoin(page_url, href).split("#")[0])
    return page_url, resolved


def main():
    scan_all = "--all" in sys.argv
    scan_api = "--api-only" in sys.argv

    print("MkDocs 404 Link Scanner")
    print(f"Target: {BASE_URL}")
    if scan_all:
        print("Mode: all pages (including API)")
    elif scan_api:
        print("Mode: API pages only")
    else:
        print("Mode: HandBook pages only (use --all for full scan)")
    print("=" * 60)

    # --- Phase 1: Collect all links ---
    print("\n[Phase 1] Collecting links from all pages...")
    sitemap_url = urljoin(BASE_URL, "sitemap.xml")
    status, sitemap_xml = fetch(sitemap_url)
    if status != 200:
        print(f"ERROR: Cannot fetch sitemap (HTTP {status})")
        print("Make sure MkDocs dev server is running: mkdocs serve")
        sys.exit(1)

    page_urls = re.findall(r"<loc>(.*?)</loc>", sitemap_xml)
    page_urls = [u for u in page_urls if BASE_URL.rstrip("/") in u and "sitemaps.org" not in u]

    if not scan_all and not scan_api:
        page_urls = [u for u in page_urls if "/api/" not in u]
    elif scan_api:
        page_urls = [u for u in page_urls if "/api/" in u]

    print(f"  Pages to scan: {len(page_urls)}")

    # url_to_sources: resolved_url -> set of source pages
    url_to_sources = defaultdict(set)
    # raw_href_map: resolved_url -> set of raw href strings
    raw_href_map = defaultdict(set)

    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as pool:
        futures = {pool.submit(collect_links, url): url for url in page_urls}
        done = 0
        for future in as_completed(futures):
            done += 1
            if done % 20 == 0 or done == len(page_urls):
                print(f"\r  Collecting: [{done * 100 // len(page_urls):3d}%] {done}/{len(page_urls)}", end="", flush=True)
            page_url, links = future.result()
            for link in links:
                url_to_sources[link].add(page_url)

    print(f"\n  Found {len(url_to_sources)} unique URLs to check")

    # --- Phase 2: Check each unique URL exactly once ---
    print(f"\n[Phase 2] Checking {len(url_to_sources)} unique URLs...")
    broken_targets = {}  # url -> status_code

    unique_urls = list(url_to_sources.keys())
    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as pool:
        futures = {pool.submit(fetch, url): url for url in unique_urls}
        done = 0
        for future in as_completed(futures):
            done += 1
            if done % 50 == 0 or done == len(unique_urls):
                print(f"\r  Checking: [{done * 100 // len(unique_urls):3d}%] {done}/{len(unique_urls)}", end="", flush=True)
            url = futures[future]
            link_status, _ = future.result()
            if link_status == 404:
                broken_targets[url] = link_status

    print()

    # --- Phase 3: Report ---
    print(f"\n[Phase 3] Results")
    print("=" * 60)

    if not broken_targets:
        print("\n  All internal links are valid. No 404s found.")
        sys.exit(0)

    print(f"\n  Found {len(broken_targets)} broken target(s):\n")

    for i, target in enumerate(sorted(broken_targets), 1):
        sources = sorted(url_to_sources[target])
        print(f"  {i}. 404 --> {target}")
        print(f"     Referenced from {len(sources)} page(s):")
        for src in sources[:5]:
            print(f"       - {src}")
        if len(sources) > 5:
            print(f"       ... and {len(sources) - 5} more")
        print()

    print("=" * 60)
    print(f"  Broken targets: {len(broken_targets)}")
    print(f"  Total references: {sum(len(url_to_sources[t]) for t in broken_targets)}")

    sys.exit(1)


if __name__ == "__main__":
    main()
