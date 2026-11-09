# Como restaurar o SINGULARITY e o Sabotaj no novo PC

**Estado: restauração incompleta. NÃO FORMATAR O PC ORIGINAL ainda.** Este repositório contém apenas parte do trabalho.

## SINGULARITY (Unreal Engine 5.8)

1. Clone este repositório e copie a subpasta `SINGULARITY/` para uma pasta de projetos Unreal no novo computador.
2. Os arquivos `SINGULARITY.uproject`, `Source/`, `Config/` e parte de `Content/` estão preservados, mas **nem todos os assets binários**.
3. Extraia `Archives/SINGULARITY_ExternalActors_Objects.zip` **dentro da pasta `Content/`**, permitindo a criação de `Content/__ExternalActors__` e `Content/__ExternalObjects__`. Não extraia o ZIP na raiz do projeto.
4. Instale a Unreal Engine **5.8** compatível, a toolchain C++/Visual Studio e os plugins exigidos pelo `.uproject`. O código C++ atual está em `Source/SINGULARITY/`.
5. **Antes de abrir o projeto, restaure separadamente** os recursos ainda ausentes em `Content/`, `SourceAssets/`, e quaisquer dependências/licenças aplicáveis. Os arquivos recuperados do Sabotaj não foram publicados aqui.
6. O histórico original da pasta `.git` também não foi integralmente migrado; os arquivos publicados são uma fotografia da árvore de trabalho, não uma reprodução completa dos commits locais.

## Sabotaj reconstruído (Unreal Engine 4.21)

1. Existe um esqueleto em `Sabotaj-UE4/` contendo `Sabotaj.uproject`, configurações básicas e scripts.
2. **Faltam** os diretórios de recursos do UE4 (`Content/`, incluindo Galata, Blueprints, armas e mapas), o conjunto completo de assets extraídos, bem como os arquivos grandes da recuperação.
3. Esse esqueleto **não é jogável nem substitui a cópia que ainda está no PC original**.

## Segurança e integridade

- O arquivo `SINGULARITY/HANDOFF_PUBLIC.md` é uma versão sanitizada de referência; o original local não foi enviado.
- Os arquivos binários que constam no GitHub tiveram seus identificadores de blob Git comparados aos arquivos locais após o upload.
- SHA-256 do ZIP dos atores externos: `FAE2CF470F7ECC6959BDD19AC6E2C58D453CBC723DA6F9C77A8EDCA2FCE4FFBF`.
- **Chaves AES, credenciais e arquivos privados não foram enviados ao repositório público**. Guardar esses elementos em local seguro, criptografado e verificável antes de formatar o PC é indispensável.
- Uma restauração completa exige testar o projeto em uma máquina diferente antes de apagar a original.

Consulte `MIGRATION_STATUS.md` para o inventário publicado e o que ainda falta.
