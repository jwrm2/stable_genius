.section .text
# Loads a Page Directory Table into the cr3 register
# Physical address of PDT at %esp + 4
.global load_pdt
load_pdt:
    mov 4(%esp), %eax
    mov %eax, %cr3
    ret

# Enables paging, by setting the highest bit of the cr0 register
.global enable_paging
enable_paging:
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    ret

# Enables Page Size Extension (PSE), ie 4MB pages
.global enable_pse
enable_pse:
    mov %cr4, %eax
    or $0x00000010, %eax
    mov %eax, %cr4
    ret

# Invalidates the page at the given virtual address.
# Address to invalidate at %esp + 4
.global invalidate_page
invalidate_page:
    mov 4(%esp), %eax
    invlpg (%eax)
    ret

# Fetch the contents of the CR2 register.
.global get_cr2
get_cr2:
    mov %cr2, %eax
    ret
