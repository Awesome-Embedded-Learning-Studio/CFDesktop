export interface SidebarVolume {
  name: string;
  srcDir: string;
  urlPrefix: string;
}

export interface ProjectConfig {
  name: string;
  title: string;
  description: string;
  base: string;
  documentsDir: string;
  github: {
    owner: string;
    repo: string;
    branch: string;
    documentsPath: string;
  };
  nav: Array<{ text: string; link: string }>;
  sidebar: {
    volumes: SidebarVolume[];
  };
}

const projectConfig: ProjectConfig = {
  name: "CFDesktop",
  title: "CFDesktop 文档",
  description: "面向嵌入式设备的 Qt / Material Design 3 桌面框架",
  base: "/CFDesktop/",
  documentsDir: "document",
  github: {
    owner: "Awesome-Embedded-Learning-Studio",
    repo: "CFDesktop",
    branch: "develop",
    documentsPath: "document"
  },
  nav: [
    { text: "首页", link: "/" },
    { text: "状态", link: "/status/current" },
    { text: "开发", link: "/development/" },
    { text: "架构", link: "/design_stage/system_architecture_overview" },
    { text: "桌面路线图", link: "/todo/desktop/" },
    { text: "手册", link: "/HandBook/" },
    { text: "GitHub", link: "https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop" }
  ],
  sidebar: {
    volumes: [
      { name: "development", srcDir: "development", urlPrefix: "/development" },
      { name: "status", srcDir: "status", urlPrefix: "/status" },
      { name: "ci", srcDir: "ci", urlPrefix: "/ci" },
      { name: "architecture", srcDir: "design_stage", urlPrefix: "/design_stage" },
      { name: "desktop", srcDir: "desktop", urlPrefix: "/desktop" },
      { name: "handbook", srcDir: "HandBook", urlPrefix: "/HandBook" },
      { name: "todo", srcDir: "todo", urlPrefix: "/todo" },
      { name: "scripts", srcDir: "scripts", urlPrefix: "/scripts" },
      { name: "notes", srcDir: "notes", urlPrefix: "/notes" },
      { name: "release", srcDir: "release_rule", urlPrefix: "/release_rule" }
    ]
  }
};

export default projectConfig;
