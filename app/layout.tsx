import type { Metadata } from 'next'
import { Inter } from 'next/font/google'
import './globals.css'
import PWAPrompt from '@/components/PWAPrompt'

const inter = Inter({ subsets: ['latin'] })

export const metadata: Metadata = {
  title: 'Dashboard pH Metro',
  description: 'Sistema de monitoreo de pH en tiempo real',
  manifest: '/manifest.json',
  themeColor: '#3b82f6',
  viewport: 'width=device-width, initial-scale=1, maximum-scale=1',
}

export default function RootLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <html lang="es">
      <body className={inter.className}>
        <main className="min-h-screen bg-gray-50">
          {children}
        </main>
        <PWAPrompt />
      </body>
    </html>
  )
}
