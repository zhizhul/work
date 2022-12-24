#include <iostream>
#include "aide_conf.h"
#include "aide_itrc.h"
#include "aide_data.h"
#include "func_create.h"
#include "func_adjust.h"
#include "func_erase.h"
#include "func_finish.h"
#include "func_date.h"
#include "func_list.h"
#include "func_help.h"

using namespace std;

// 主程序入口
int main(int argc, char *argv[])
{
    // 首先由Interactor类解析命令行，分析出是什么指令，
    // 然后根据指令来展开操作。
    int type = Interactor::GetFunctionType(argc, argv);
    if (type == Function::create)
    {
        // 创建项目功能，不另外接收参数，会和用户交互并要求输入
        // 项目的名称、信息、计划日期。
        FuncCreate create;
        create.execute();
    }
    else if (type == Function::adjust)
    {
        // 调整项目功能，要另外接收2个参数，一个是项目ID，另
        // 一个是要修改的项。
	FuncAdjust adjust;
	adjust.execute(argc, argv);
    }
    else if (type == Function::erase)
    {
        // 删除项目功能，要接收项目名称ID，并且要确认。
	FuncErase erase;
	erase.execute(argc, argv);
    }
    else if (type == Function::finish)
    {
        // 完成项目功能，要接收项目名称ID，并且要确认。
        FuncFinish finish;
	finish.execute(argc, argv);
    }
    else if (type == Function::date)
    {
        // 显示未完成项目功能，不另外接收参数。
        FuncDate date;
	date.execute();
    }
    else if (type == Function::list)
    {
        // 显示已完成项目功能，不另外接收参数。
        FuncList list;
	list.execute();
    }
    else if (type == Function::help)
    {
        // 帮助功能，不另外接收参数，用于弹出使用说明。
        FuncHelp help;
	help.execute();
    }
    else
    {
        cout << "False instructions, you can input --help or -h for help." << endl;
    }
    return 0;   
}
(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(custom-enabled-themes '(manoj-dark))
 '(global-display-line-numbers-mode t)
 '(inhibit-startup-screen t)
 '(package-selected-packages
   '(counsel-projectile projectile company-box company smooth-scrolling lsp-ui lsp-mode ace-window good-scroll use-package))
 '(tool-bar-mode nil))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(default ((t (:family "DejaVu Sans Mono" :foundry "PfEd" :slant normal :weight normal :height 158 :width normal)))))
 ;; Auto add pair
(electric-pair-mode t)
 ;; Set type bar
(setq-default cursor-type 'bar)
 ;; Disable files backup function
(setq make-backup-files nil)
 ;; Close tool bar
(tool-bar-mode -1)
 ;; Auto sync changes from other processes
(global-auto-revert-mode t)
 ;; Show line numbers
(global-display-line-numbers-mode 1)
 ;; Adjust indent mode
(defun return-and-indent-relative()
  (interactive)
  (newline)
  (indent-relative-first-indent-point))
(global-set-key (kbd "RET") 'return-and-indent-relative)
(global-set-key (kbd "C-m") 'return-and-indent-relative)
 ;; Adjust backspace mode
(global-set-key (kbd "C-h") 'delete-backward-char)
(global-set-key (kbd "M-h") 'backward-kill-word)
 ;; Adjust newline mode
(defun create-newline-and-indent()
  (interactive)
  (end-of-line)
  (newline)
  (indent-relative-first-indent-point))
(global-set-key (kbd "C-o") 'create-newline-and-indent)
 ;; Adjust C++ coding style
(setq c-default-style "linux" c-basic-offset 4)
 ;; Set package list
(require 'package)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
(setq package-archives '(("gnu" . "http://mirrors.cloud.tencent.com/elpa/gnu/")
                         ("melpa" . "http://mirrors.cloud.tencent.com/elpa/melpa/")))
(package-initialize)
 ;; use-package plug-in unit
(eval-when-compile
  (require 'use-package))
 ;; ivy
(use-package counsel
  :ensure t)
(use-package ivy
  :ensure t
  :init
  (ivy-mode 1)
  (counsel-mode 1)
  :config
  (setq ivy-use-virtual-buffers t)
  (setq search-default-mode #'char-fold-to-regexp)
  (setq ivy-count-format "(%d/%d) ")
  :bind
  (("C-s" . 'swiper)
   ("C-x b" . 'ivy-switch-buffer)
   ("C-c v" . 'ivy-push-view)
   ("C-c s" . 'ivy-switch-view)
   ("C-c V" . 'ivy-pop-view)
   ("C-x C-@" . 'counsel-mark-ring)
   ("C-x C-SPC" . 'counsel-mark-ring)
   :map minibuffer-local-map
   ("C-r" . counsel-minibuffer-history)))
 ;; good-scroll plug-in unit
(use-package good-scroll
  :ensure t
  :if window-system
  :init (good-scroll-mode))
 ;; ace-window plug-in unit
(use-package ace-window
  :ensure t
  :bind (("C-x o" . 'ace-window)))
 ;; smooth-scrolling plug-in unit
(use-package smooth-scrolling
  :ensure t
  :init (smooth-scrolling-mode)
  :config
  (setq smooth-scroll-margin 3))
 ;; company
(use-package company
 :ensure t
 :init (global-company-mode)
 :config
 (setq company-minimum-prefix-length 1)
 (setq company-tooltip-align-annotations t)
 (setq company-idle-delay 0.0)
 (setq company-show-numbers t)
 (setq company-selection-wrap-around t)
 (setq company-transformers '(company-sort-by-occurrence)))
(use-package company-box
 :ensure t
 :if window-system
 :hook (company-mode . company-box-mode))
 ;; lsp
(use-package lsp-mode
  :ensure t
  :init
  (setq lsp-keymap-prefix "C-c l"
	lsp-file-watch-threshold 500)
  :hook 
  (lsp-mode . lsp-enable-which-key-integration)
  :commands (lsp lsp-deferred)
  :config
    (setq lsp-completion-provider :none)
    (setq lsp-headerline-breadcrumb-enable t))
 ;; projectile
(use-package projectile
  :ensure t
  :bind (("C-c p" . projectile-command-map))
  :config
  (setq projectile-mode-line "Projectile")
  (setq projectile-track-known-projects-automatically nil))
(use-package counsel-projectile
  :ensure t
  :after (projectile)
  :init (counsel-projectile-mode))
(use-package c++-mode
  :functions 			
  c-toggle-hungry-state
  :hook
  (c-mode . lsp-deferred)
  (c++-mode . lsp-deferred)
  (c++-mode . c-toggle-hungry-state))
