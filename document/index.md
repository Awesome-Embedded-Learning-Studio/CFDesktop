---
layout: home

hero:
  name: "CFDesktop"
  text: "嵌入式桌面框架"
  tagline: 基于 Qt 6 / C++23 / Material Design 3 的跨平台桌面 Shell 实验场
  image:
    src: /Awesome-Embedded.png
    alt: CFDesktop Logo
  actions:
    - theme: brand
      text: 开始开发
      link: /development/
    - theme: alt
      text: 当前状态
      link: /status/current
    - theme: alt
      text: 桌面路线图
      link: /todo/desktop/

features:
  - title: 三层架构
    details: base、ui、desktop 单向依赖，分别承载基础库、Material UI 框架和桌面 Shell。
    link: /design_stage/system_architecture_overview
  - title: Material Design 3
    details: 已有主题、Token、动画、行为层和 P0/P1 控件基础，适合作为 Shell UI 底座。
    link: /HandBook/ui/
  - title: 可见桌面闭环
    details: 下一阶段优先推进状态栏、任务栏、应用启动器和窗口管理联动。
    link: /todo/desktop/milestone_00_overview
  - title: VitePress 文档站
    details: 文档系统已迁移到 VitePress，使用 npm 脚本和 GitHub Actions 构建发布。
    link: /scripts/document/
---
