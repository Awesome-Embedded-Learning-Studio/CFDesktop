import { defineConfig } from "vitepress";

export default defineConfig({
  title: "CFDesktop",
  description: "A Qt-based Material Design 3 desktop framework for embedded devices.",
  base: "/CFDesktop/",
  cleanUrls: true,
  lastUpdated: true,
  ignoreDeadLinks: true,
  srcExclude: ["api/**"],
  markdown: {
    html: false
  },
  themeConfig: {
    nav: [
      { text: "Start Here", link: "/" },
      { text: "Development", link: "/development/" },
      { text: "Architecture", link: "/design_stage/system_architecture_overview" },
      { text: "Desktop Roadmap", link: "/todo/desktop/" },
      { text: "HandBook", link: "/HandBook/" }
    ],
    sidebar: [
      {
        text: "Start Here",
        items: [
          { text: "Overview", link: "/" },
          { text: "Current TODO", link: "/todo/" }
        ]
      },
      {
        text: "Development",
        items: [
          { text: "Guide", link: "/development/" },
          { text: "Prerequisites", link: "/development/01_prerequisites" },
          { text: "Quick Start", link: "/development/02_quick_start" },
          { text: "Build System", link: "/development/03_build_system" },
          { text: "CI", link: "/ci/" }
        ]
      },
      {
        text: "Architecture",
        items: [
          { text: "System Overview", link: "/design_stage/system_architecture_overview" },
          { text: "Display Backend", link: "/design_stage/multi_display_backend_architecture" },
          { text: "Desktop", link: "/desktop/" }
        ]
      },
      {
        text: "Roadmap",
        items: [
          { text: "Desktop Overview", link: "/todo/desktop/" },
          { text: "Milestones", link: "/todo/desktop/milestone_00_overview" },
          { text: "Done", link: "/todo/done/" }
        ]
      },
      {
        text: "HandBook",
        items: [
          { text: "HandBook", link: "/HandBook/" },
          { text: "Base", link: "/HandBook/base/" },
          { text: "UI", link: "/HandBook/ui/" },
          { text: "Desktop", link: "/HandBook/desktop/" }
        ]
      }
    ],
    socialLinks: [
      { icon: "github", link: "https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop" }
    ],
    search: {
      provider: "local"
    }
  }
});
