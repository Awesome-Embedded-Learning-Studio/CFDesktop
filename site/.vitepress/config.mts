import { existsSync, readdirSync, readFileSync, statSync } from "node:fs";
import { join } from "node:path";
import { fileURLToPath } from "node:url";
import { defineConfig, type DefaultTheme } from "vitepress";
import projectConfig from "../../project.config";

type SidebarItem = DefaultTheme.SidebarItem;

const docsRoot = fileURLToPath(new URL(`../../${projectConfig.documentsDir}`, import.meta.url));
const githubUrl = `https://github.com/${projectConfig.github.owner}/${projectConfig.github.repo}`;
const editPattern = `${githubUrl}/edit/${projectConfig.github.branch}/${projectConfig.github.documentsPath}/:path`;

function extractTitle(filePath: string): string | null {
  try {
    const content = readFileSync(filePath, "utf-8");
    const frontmatterTitle = content.match(/^---[\s\S]*?^title:\s*["']?(.+?)["']?\s*$/m);
    if (frontmatterTitle) return frontmatterTitle[1];
    const h1 = content.match(/^#\s+(.+)$/m);
    if (h1) return h1[1].replace(/\{.*?\}/g, "").trim();
  } catch {
    return null;
  }
  return null;
}

function humanize(name: string): string {
  return name
    .replace(/^\d+[-_]?/, "")
    .replace(/[-_]/g, " ")
    .replace(/\b\w/g, (c) => c.toUpperCase());
}

function sortEntries(a: string, b: string): number {
  const aIndex = a.match(/^(\d+)/)?.[1];
  const bIndex = b.match(/^(\d+)/)?.[1];
  if (aIndex && bIndex) return Number(aIndex) - Number(bIndex);
  if (aIndex) return -1;
  if (bIndex) return 1;
  if (a === "index.md") return -1;
  if (b === "index.md") return 1;
  return a.localeCompare(b, "en");
}

function shouldSkip(name: string): boolean {
  return (
    name.startsWith(".") ||
    name === "api" ||
    name === "stylesheets" ||
    name === "Awesome-Embedded.png" ||
    name === "Awesome-Embedded.ico"
  );
}

function scanDir(dir: string, urlPrefix: string, depth = 0): SidebarItem[] {
  if (depth > 5) return [];

  let entries: string[];
  try {
    entries = readdirSync(dir).filter((entry) => !shouldSkip(entry));
  } catch {
    return [];
  }

  entries.sort(sortEntries);
  const items: SidebarItem[] = [];

  for (const name of entries) {
    const fullPath = join(dir, name);
    const stat = statSync(fullPath);

    if (stat.isDirectory()) {
      const indexPath = join(fullPath, "index.md");
      const children = scanDir(fullPath, `${urlPrefix}/${name}`, depth + 1);
      const title = extractTitle(indexPath) || humanize(name);

      if (children.length > 0) {
        items.push({
          text: title,
          link: existsSync(indexPath) ? `${urlPrefix}/${name}/` : undefined,
          collapsed: depth > 0,
          items: children
        });
      } else if (existsSync(indexPath)) {
        items.push({ text: title, link: `${urlPrefix}/${name}/` });
      }
      continue;
    }

    if (name.endsWith(".md") && name !== "index.md" && name !== "README.md") {
      const title = extractTitle(fullPath) || humanize(name.replace(/\.md$/, ""));
      items.push({ text: title, link: `${urlPrefix}/${name.replace(/\.md$/, "")}` });
    }
  }

  return items;
}

function volumeSidebar(srcDir: string, urlPrefix: string): SidebarItem[] {
  const dir = join(docsRoot, srcDir);
  const indexPath = join(dir, "index.md");
  const title = extractTitle(indexPath) || humanize(srcDir);
  return [{ text: title, link: `${urlPrefix}/` }, ...scanDir(dir, urlPrefix)];
}

function buildSidebar(): DefaultTheme.Sidebar {
  const sidebar: DefaultTheme.Sidebar = {};
  for (const volume of projectConfig.sidebar.volumes) {
    sidebar[`${volume.urlPrefix}/`] = volumeSidebar(volume.srcDir, volume.urlPrefix);
  }
  return sidebar;
}

export default defineConfig({
  srcDir: `../${projectConfig.documentsDir}`,
  title: projectConfig.title,
  description: projectConfig.description,
  lang: "zh-CN",
  base: projectConfig.base,
  cleanUrls: true,
  lastUpdated: true,
  ignoreDeadLinks: true,
  srcExclude: ["api/**"],
  head: [["link", { rel: "icon", href: `${projectConfig.base}Awesome-Embedded.ico` }]],
  markdown: {
    html: false,
    lineNumbers: true,
    theme: {
      light: "github-light",
      dark: "github-dark"
    }
  },
  themeConfig: {
    nav: projectConfig.nav,
    sidebar: buildSidebar(),
    search: {
      provider: "local"
    },
    editLink: {
      pattern: editPattern,
      text: "在 GitHub 上编辑此页"
    },
    footer: {
      message: "Built with VitePress",
      copyright: "Copyright © 2026 CharlieChen"
    },
    socialLinks: [{ icon: "github", link: githubUrl }]
  },
  vite: {
    publicDir: fileURLToPath(new URL("./public", import.meta.url)),
    build: {
      chunkSizeWarningLimit: 5000
    }
  }
});
