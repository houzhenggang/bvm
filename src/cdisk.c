#include "cdisk.h"
#include "zfs.h"

create_stru diskmenu[DM_MAX] = {0};
create_stru *disk_sel[DM_MAX] = {0};
//void (*disk_sel[DM_MAX])() = {0};

// 磁盘菜单初始化
void disk_config_init()
{
	static int flag = 1;
	if (flag) {
		flag = 0;
		add_item(diskmenu, "disk number",  (char*)&new_vm.disks, 	enter_vm_disks,    	0, 1, 0);
		for (int n=0; n<DISK_NUM; n++) {
			char desc[BUFFERSIZE];
			sprintf(desc, "disk(%d) size", n);
			add_item(diskmenu, desc, (char*)&new_vm.vdisk[n].size, enter_vm_vdisk_size, 	n, 1, 0);
		}
		add_item(diskmenu, "ZFS support",  (char*)&new_vm.zfs,		enter_vm_zfs,		0, 1, 0);
		add_item(diskmenu, "ZFS pool",     (char*)&new_vm.zpool,	enter_vm_zpool, 	0, 1, 0);
		add_item(diskmenu, "add a disk",   NULL, 			add_disk,    		0, 1, 1);
		add_item(diskmenu, "delete a disk",NULL, 			delete_disk, 		0, 1, 1);
		add_item(diskmenu, "go back", 	   NULL, 	     		goback_mainmenu, 	0, 1, 1);
	}
}

// 建立磁盘配置
void create_disk_config()
{
	if (atoi(new_vm.disks) == 0) {
		int n = 0;
		while (diskmenu[n].func && diskmenu[n].submenu == 0) {
			diskmenu[n].func(diskmenu[n].arg);
			n++;
		}
	}

	edit_disk_config();
}

// 编辑磁盘配置
void edit_disk_config()
{
	vm_node *p;
	if ((p = find_vm_list(new_vm.name)) != NULL) {
		set_disk_edit(DISKETTE, 0);
		set_disk_edit(DISKFUNC, 1);
	}
	else {
		set_disk_edit(DISKETTE, 1);
		set_disk_edit(DISKFUNC, -1);
	}

	char *msg = "Enter an number: ";
	show_disk_config();

	int min = 0, max = 0;
	char answer[8];
	while (1) {
		while (disk_sel[max]) ++max;
		--max;

		printf("%s", msg);
		fgets(answer, sizeof(answer), stdin);
		answer[strlen(answer)-1] = '\0';

		//int n = strtoint(answer);
		int n = check_options(min, max, answer);
		if (n < min || n > max) {
			printf("\033[1A\033[K"); 
			continue;
		}

		if (disk_sel[n]->func == goback_mainmenu) {
			if (check_disk_enter_valid() == -1) { 
				printf("\033[1A\033[K"); 
				continue; 
			}
			break;
		}
		if (!is_edit_item(diskmenu, disk_sel, n)) { 
			printf("\033[1A\033[K"); 
			continue; 
		}
		if (disk_sel[n]) disk_sel[n]->func(disk_sel[n]->arg);
		show_disk_config();
	}
}

// 设置磁盘菜单项的编辑属性
void set_disk_edit(int type, int edit)
{
	int n = DISK_NUM + 3;	//+3是3个菜单项(vm_disks,vm_zfs,vm_zpool)
	if (type == DISKETTE)
		for (int i=0; i<n; i++)
			diskmenu[i].edit = edit;
	if (type == DISKFUNC)
		for (int i=n; i<n+2; i++)
			diskmenu[i].edit = edit;
}

// 检测磁盘输入有效性
int check_disk_enter_valid()
{
	for (int i=0; i<atoi(new_vm.disks); i++) {
		//if (strlen(&new_vm.imgsize+i*(&new_vm.img1size-&new_vm.imgsize)) == 0)
		if (strlen(new_vm.vdisk[i].size) == 0)
			return -1;
	}
	return 1;
}

// 显示磁盘配置
void show_disk_config()
{
	int n = 0;
	int index = 0;
	while (diskmenu[n].func) {
		if ((diskmenu[n].func == enter_vm_vdisk_size && atoi(new_vm.disks) < diskmenu[n].arg + 1) ||
		/*		
		if ((diskmenu[n].func == enter_vm_img1size && atoi(new_vm.disks) < 2) ||
		   (diskmenu[n].func == enter_vm_img2size && atoi(new_vm.disks) < 3) ||
		   (diskmenu[n].func == enter_vm_img3size && atoi(new_vm.disks) < 4) ||
		   (diskmenu[n].func == enter_vm_img4size && atoi(new_vm.disks) < 5) ||
		   (diskmenu[n].func == enter_vm_img5size && atoi(new_vm.disks) < 6) ||
		   (diskmenu[n].func == enter_vm_img6size && atoi(new_vm.disks) < 7) ||
		   (diskmenu[n].func == enter_vm_img7size && atoi(new_vm.disks) < 8) ||
		  */
		   (diskmenu[n].func == enter_vm_zfs && !support_zfs()) ||
		   (diskmenu[n].func == enter_vm_zpool && strcmp(new_vm.zfs, "on") != 0));
		else {
			if (diskmenu[n].edit != -1) {
			disk_sel[index] = &diskmenu[n];
			//printf("[%2d]. %-13s", index, diskmenu[n].desc);
			printf("[%c]. %-13s", options[index], diskmenu[n].desc);
			if (diskmenu[n].value)
				printf(": %s", diskmenu[n].value);
			printf("\n");
			++index;
			}
		}
		++n;
	}
}

// 增加磁盘
void add_disk(int not_use)
{
	warn("... add disk ...\n");
	vm_add_disk(new_vm.name);
	load_vm_info(new_vm.name, &new_vm);
}

// 删除磁盘
void delete_disk(int not_use)
{
	warn("... delete disk ...\n");
	vm_del_disk(new_vm.name);
	load_vm_info(new_vm.name, &new_vm);
}

