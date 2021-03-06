/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef LOG_H
#define LOG_H

/**
 * @brief Defaul module <code>printk</code> format
 */
#define pr_fmt(fmt) "%s:%s: " fmt, KBUILD_MODNAME, __func__

#endif /* LOG_H */
