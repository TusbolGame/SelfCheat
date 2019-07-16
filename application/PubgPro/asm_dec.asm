

.CODE



public asm_decrypt_component
asm_decrypt_component PROC
			   sub 	rsp,20h
			   mov  rdx,rcx
               movzx   eax, dx
               mov     ecx, edx
               mov     r8, rdx
               shr     ecx, 10h
               xor     ecx, eax
               shr     r8, 20h
               and     edx, 0FFFF0000h
               mov     qword ptr [rsp], 0
               or      ecx, edx
               add     ecx, 28288858h
               movzx   eax, cx
               mov     edx, ecx
               shr     edx, 10h
               and     ecx, 0FFFF0000h
               xor     edx, eax
               or      edx, ecx
               xor     edx, 28288858h
               mov     dword ptr [rsp], edx
               mov     edx, r8d
               shr     edx, 10h
               movzx   eax, dx
               xor     dx, r8w
               ror     ax, 8
               movzx   ecx, ax
               rol     dx, 8
               movzx   eax, dx
               shl     ecx, 10h
               or      ecx, eax
               add     ecx, 27572758h
               mov     edx, ecx
               shr     edx, 10h
               movzx   eax, dx
               xor     dx, cx
               ror     ax, 8
               movzx   r8d, ax
               rol     dx, 8
               shl     r8d, 10h
               movzx   eax, dx
               or      r8d, eax
               xor     r8d, 0D8A8D8A8h
               mov     dword ptr [rsp+4], r8d
               mov     rax, qword ptr [rsp]
			   add 	   rsp,20h
			   ret
asm_decrypt_component ENDP
END
